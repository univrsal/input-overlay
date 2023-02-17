/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2022 univrsal <uni@vrsal.xyz>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#include "network_helper.hpp"
#include "client_util.hpp"
#include "uiohook_helper.hpp"
#include <mongoose.h>
#include <buffer.hpp>
#include <atomic>
#include <thread>
#include <cstdio>

namespace network_helper {
buffer buf;
std::atomic<bool> status{false}, connected{false};
std::thread network_thread;
std::deque<buffer> message_queue;
std::mutex poll_mutex;
struct mg_mgr mgr;

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    if (ev == MG_EV_ERROR) {
        berr("%p %s", c->fd, (char *)ev_data);
    } else if (ev == MG_EV_WS_OPEN) {
        // When websocket handshake is successful, send message
        connected = true;
        mg_ws_send(c, "hello", 5, WEBSOCKET_OP_TEXT);
    } else if (ev == MG_EV_WS_MSG) {
        struct mg_ws_message *wm = (struct mg_ws_message *)ev_data;
        binfo("WSS: [%.*s]\n", (int)wm->data.len, wm->data.ptr);
    }

    if (ev == MG_EV_ERROR || ev == MG_EV_CLOSE) {
        status = false;
        uiohook_helper::stop();
    }
}

bool start()
{
    mg_log_set_callback(
        [](const void *data, size_t len, void *) {
            if (len > 0 && strcmp((char *)data, "\n") != 0)
                binfo("%.*s", (int)len, (char *)data);
        },
        nullptr);
    if (status)
        return true;
    status = true;
    network_thread = std::thread([] {
        struct mg_connection *c;
        mg_mgr_init(&mgr);
        c = mg_ws_connect(&mgr, util::cfg.websocket_address, fn, nullptr, nullptr);
        while (c && status) {
            mg_mgr_poll(&mgr, 50);
            // Send events
            poll_mutex.lock();
            while (!message_queue.empty()) {
                auto &msg = message_queue.back();
                if (!c->is_draining && !c->is_closing)
                    mg_ws_send(c, msg.get<const char *>(), msg.length(), WEBSOCKET_OP_BINARY);
                message_queue.pop_back();
            }
            poll_mutex.unlock();
        }
    });
    return !!network_thread.native_handle();
}

void stop()
{
    status = false;
    connected = false;
    network_thread.join();
    binfo("network stopped");
}
}
