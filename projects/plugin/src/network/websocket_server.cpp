#include "websocket_server.hpp"
#include "../util/config.hpp"
#include "../util/settings.h"
#include "../util/log.h"
#include "../util/obs_util.hpp"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <mongoose.h>
#include <thread>
#include <mutex>
#include <deque>
#include <atomic>
#include <util/threading.h>

namespace wss {
std::atomic<bool> thread_flag;
struct mg_mgr mgr;
struct mg_connection *nc;
std::mutex poll_mutex;
std::thread thread_handle;
std::vector<struct mg_connection *> sockets;
std::deque<std::string> message_queue;

void event_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    if (ev == MG_EV_HTTP_MSG) {
        auto *hm = (struct mg_http_message *)ev_data;
        if (mg_http_match_uri(hm, "/websocket")) {
            // Upgrade to websocket. From now on, a connection is a full-duplex
            // Websocket connection, which will receive MG_EV_WS_MSG events.
            mg_ws_upgrade(c, hm, nullptr);
            sockets.emplace_back(c);
        }
    } else if (ev == MG_EV_WS_MSG) {
        // Just echo data
        auto *wm = (struct mg_ws_message *)ev_data;
        mg_ws_send(c, wm->data.ptr, wm->data.len, WEBSOCKET_OP_TEXT);
        mg_iobuf_delete(&c->recv, c->recv.len);
    }
}

bool start()
{
    if (thread_flag)
        return true;
    thread_flag = true;

    bool result = true;
    auto port = CGET_INT(S_WSS_PORT);
    std::string url = "http://localhost:";
    url = url.append(std::to_string(port));
    binfo("Starting web server on %ld", port);
    mg_mgr_init(&mgr);
    nc = mg_listen(&mgr, url.c_str(), event_handler, nullptr);

    if (!nc) {
        berr("Failed to start listener");
        result = false;
    }

    thread_handle = std::thread([] {
        os_set_thread_name("input-overlay-wss");

        for (;;) {
            poll_mutex.lock();
            mg_mgr_poll(&mgr, 100);
            while (!message_queue.empty()) {
                auto &msg = message_queue.back();
                for (auto socket : sockets)
                    mg_ws_send(socket, msg.c_str(), msg.length(), WEBSOCKET_OP_TEXT);
                message_queue.pop_back();
            }
            poll_mutex.unlock();
            if (!thread_flag)
                break;
        }
    });
    if (thread_handle.native_handle())
        result = false;

    thread_flag = result;
    return result;
}

void stop()
{
    if (!thread_flag)
        return;
    binfo("Stopping web server running on %ld", CGET_INT(S_WSS_PORT));

    thread_flag = false;
    thread_handle.join();
    mg_mgr_free(&mgr);
}

QString serialize_uiohook(const uiohook_event *e, const std::string &source_name)
{
    QJsonObject obj;
    auto ev_to_str = [](int e) {
        switch (e) {
        case EVENT_KEY_TYPED:
            return "key_typed";
        case EVENT_KEY_PRESSED:
            return "key_pressed";
        case EVENT_KEY_RELEASED:
            return "key_released";
        case EVENT_MOUSE_CLICKED:
            return "mouse_clicked";
        case EVENT_MOUSE_PRESSED:
            return "mouse_pressed";
        case EVENT_MOUSE_RELEASED:
            return "mouse_released";
        case EVENT_MOUSE_MOVED:
            return "mouse_moved";
        case EVENT_MOUSE_DRAGGED:
            return "mouse_dragged";
        case EVENT_MOUSE_WHEEL:
            return "mouse_wheel";
        default:
            return "";
        }
    };

    switch (e->type) {
    case EVENT_KEY_TYPED:
    case EVENT_KEY_PRESSED:
    case EVENT_KEY_RELEASED:
        obj["event_source"] = utf8_to_qt(source_name.c_str());
        obj["event_type"] = QString(ev_to_str(e->type));
        obj["time"] = int(e->time);
        obj["mask"] = e->mask;
        obj["keycode"] = e->data.keyboard.keycode;
        obj["rawcode"] = e->data.keyboard.rawcode;
        obj["char"] = QString(e->data.keyboard.keychar);
        break;
    case EVENT_MOUSE_CLICKED:
    case EVENT_MOUSE_PRESSED:
    case EVENT_MOUSE_RELEASED:
    case EVENT_MOUSE_MOVED:
    case EVENT_MOUSE_DRAGGED:
        obj["event_source"] = utf8_to_qt(source_name.c_str());
        obj["event_type"] = QString(ev_to_str(e->type));
        obj["time"] = int(e->time);
        obj["mask"] = e->mask;
        obj["button"] = e->data.mouse.button;
        obj["clicks"] = e->data.mouse.clicks;
        obj["x"] = e->data.mouse.x;
        obj["y"] = e->data.mouse.y;
        break;
    case EVENT_MOUSE_WHEEL:
        obj["event_source"] = utf8_to_qt(source_name.c_str());
        obj["event_type"] = QString(ev_to_str(e->type));
        obj["time"] = int(e->time);
        obj["mask"] = e->mask;
        obj["clicks"] = e->data.wheel.clicks;
        obj["type"] = e->data.wheel.type;
        obj["amount"] = e->data.wheel.amount;
        obj["rotation"] = e->data.wheel.rotation;
        obj["x"] = e->data.wheel.x;
        obj["y"] = e->data.wheel.y;
        break;
    default:;
    }
    QJsonDocument doc(obj);
    return QString(doc.toJson(QJsonDocument::Compact));
}

void dispatch_uiohook_event(const uiohook_event *e, const std::string &source_name)
{
    if (!thread_flag)
        return;
    std::lock_guard<std::mutex> lock(poll_mutex);
    auto json = serialize_uiohook(e, source_name);
    if (!json.isEmpty())
        message_queue.emplace_back(qt_to_utf8(json));
}

void dispatch_gamepad_event(const gamepad::input_event *e, const std::shared_ptr<gamepad::device> &device, bool is_axis,
                            const std::string &source_name)
{
    if (!thread_flag)
        return;
    QJsonObject obj;
    std::lock_guard<std::mutex> lock(poll_mutex);
    obj["event_source"] = source_name.c_str();
    obj["event_type"] = is_axis ? "gamepad_axis" : "gamepad_button";
    obj["device_name"] = utf8_to_qt(device->get_id().c_str());
    obj["device_index"] = device->get_index();
    obj["time"] = int(e->time);
    obj["virtual_code"] = e->vc;
    obj["virtual_value"] = e->virtual_value;
    obj["native_code"] = e->native_id;
    obj["native_value"] = e->value;

    QJsonDocument doc(obj);
    QString str(doc.toJson(QJsonDocument::Compact));

    if (!str.isEmpty())
        message_queue.emplace_back(qt_to_utf8(str));
}

void dispatch_gamepad_event(const std::shared_ptr<gamepad::device> &device, const char *state,
                            const std::string &source_name)
{
    if (!thread_flag)
        return;
    QJsonObject obj;
    std::lock_guard<std::mutex> lock(poll_mutex);
    obj["event_source"] = source_name.c_str();
    obj["event_type"] = state;
    obj["device_name"] = utf8_to_qt(device->get_id().c_str());
    obj["time"] = int(gamepad::hook::ms_ticks());

    QJsonDocument doc(obj);
    QString str(doc.toJson(QJsonDocument::Compact));

    if (!str.isEmpty())
        message_queue.emplace_back(qt_to_utf8(str));
}

}