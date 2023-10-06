#include "mg.hpp"

#include <atomic>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>
#include "remote_connection.hpp"
#include "../util/config.hpp"
#include "../util/log.h"
#include "../util/settings.h"

#ifdef _WIN32
#include "../util/obs_util.hpp"
#else
#include <util/threading.h>
#endif

extern "C" {
#include <mongoose.h>
}

namespace mg {
struct mg_mgr mgr {};
std::vector<struct mg_connection *> web_sockets;
std::deque<std::string> message_queue;
std::mutex poll_mutex;
std::thread thread_handle;
std::atomic<bool> thread_flag;

void event_handler(struct mg_connection *c, int ev, void *ev_data, void *)
{
    if (ev == MG_EV_HTTP_MSG) {
        auto *hm = (struct mg_http_message *)ev_data;
        if (mg_http_match_uri(hm, "/")) {
            // Upgrade to websocket. From now on, a connection is a full-duplex
            // Websocket connection, which will receive MG_EV_WS_MSG events.
            mg_ws_upgrade(c, hm, nullptr);

            if (web_sockets.empty()) // we don't want stale events
                message_queue.clear();
            web_sockets.emplace_back(c);
        }
    } else if (ev == MG_EV_WS_MSG) {
        auto *wm = (struct mg_ws_message *)ev_data;
        if (wm->flags & WEBSOCKET_OP_BINARY) {
            network::process_remote_event(c, (unsigned char *)wm->data.ptr, wm->data.len);
        }
    } else if (ev == MG_EV_CLOSE) {
        web_sockets.erase(std::remove(web_sockets.begin(), web_sockets.end(), c), web_sockets.end());
    }
}

void thread_method()
{
    os_set_thread_name("inputovrly-mg");

    while (thread_flag) {
        mg_mgr_poll(&mgr, 5);
        poll_mutex.lock();
        while (!message_queue.empty()) {
            auto &msg = message_queue.back();
            for (auto socket : web_sockets) {
                // Sockets with socket->fn_data set are remote input clients and shouldn't
                // receive any inputs
                if (!socket->is_draining && !socket->is_closing && !socket->fn_data)
                    mg_ws_send(socket, msg.c_str(), msg.length(), WEBSOCKET_OP_TEXT);
            }
            message_queue.pop_back();
        }
        poll_mutex.unlock();
    }
}

bool start(const std::string &addr)
{
    if (thread_flag)
        return true;
    thread_flag = true;
    mg_log_set_callback(
        [](const void *buf, size_t length, void *) {
            std::string str(static_cast<const char *>(buf), length);
            if (str != "\n")
                bdebug("%s", str.c_str());
        },
        nullptr);
    mg_mgr_init(&mgr);
    auto *nc = mg_http_listen(&mgr, addr.c_str(), event_handler, nullptr);
    if (!nc) {
        berr("Failed to start mongoose listener");
        return false;
    } else {
        binfo("Websocket server listening on %s", addr.c_str());
    }

    thread_handle = std::thread(thread_method);
    return true;
}

void stop()
{
    if (!thread_flag)
        return;
    binfo("Stopping web socket server running on %ld", CGET_INT(S_WSS_PORT));
    thread_flag = false;
    if (thread_handle.joinable())
        thread_handle.join();
    mg_mgr_free(&mgr);
}

void queue_message(const std::string &msg)
{
    if (!msg.empty())
        message_queue.emplace_back(msg);
}

bool can_queue_message()
{
    return thread_flag && !web_sockets.empty();
}
}
