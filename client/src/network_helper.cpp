/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.xyz/alex/input-overlay
 * Copyright 2023 univrsal <uni@vrsal.xyz>.
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
#include "gamepad_helper.hpp"
#include <mongoose.h>
#include <buffer.hpp>
#include <atomic>
#include <thread>
#include <cstdio>

namespace network_helper {
buffer buf;
std::atomic<bool> status{false}, connected{false};
std::thread network_thread;
struct mg_mgr mgr;

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    if (ev == MG_EV_ERROR) {
        berr("%p %s", c->fd, (char *)ev_data);
    } else if (ev == MG_EV_WS_OPEN) {
        // When websocket handshake is successful, send message
        connected = true;
        //        mg_ws_send(c, "hello", 5, WEBSOCKET_OP_TEXT);
    } else if (ev == MG_EV_WS_MSG) {
        //        struct mg_ws_message *wm = (struct mg_ws_message *)ev_data;
        //        binfo("WSS: [%.*s]\n", (int)wm->data.len, wm->data.ptr);
    }

    if (ev == MG_EV_ERROR || ev == MG_EV_CLOSE) {
        status = false;
        uiohook_helper::stop();
    }
}

inline const char *controller_description(int index)
{

    switch (SDL_GameControllerTypeForIndex(index)) {
    case SDL_CONTROLLER_TYPE_AMAZON_LUNA:
        return "Amazon Luna Controller";
    case SDL_CONTROLLER_TYPE_GOOGLE_STADIA:
        return "Google Stadia Controller";
#if SDL_VERSION_ATLEAST(2, 24, 0)
    case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:
    case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT:
    case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_JOYCON_PAIR:
        return "Nintendo Switch Joy-Con";
#endif
    case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_PRO:
        return "Nintendo Switch Pro Controller";
    case SDL_CONTROLLER_TYPE_PS3:
        return "PS3 Controller";
    case SDL_CONTROLLER_TYPE_PS4:
        return "PS4 Controller";
    case SDL_CONTROLLER_TYPE_PS5:
        return "PS5 Controller";
    case SDL_CONTROLLER_TYPE_XBOX360:
        return "XBox 360 Controller";
    case SDL_CONTROLLER_TYPE_XBOXONE:
        return "XBox One Controller";
    case SDL_CONTROLLER_TYPE_VIRTUAL:
        return "Virtual Game Controller";
    default:
        return "Game Controller";
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
        uint64_t last_uiohook_event = 0;
        buffer buf;
        // Roughly guess what the maximum packet size will be
        // 16bit keycodes *
        // 16 keyboard keys + 5 mouse buttons + 21 gamepad buttons for four gamepads
        // 6 gamepad axis, 64byte username and 1 byte packet type
        buf.resize(sizeof(uint16_t) * (16 + 5 + (21 * 4)) + sizeof(float) * 6 + 65);
        mg_mgr_init(&mgr);
        c = mg_ws_connect(&mgr, util::cfg.websocket_address, fn, nullptr, nullptr);

        while (c && status) {
            mg_mgr_poll(&mgr, 5);
            bool have_data = false;
            // Send events
            if (!c->is_draining && !c->is_closing) {
                buf.reset();
                buf.write(util::cfg.username);

                if (util::cfg.monitor_keyboard || util::cfg.monitor_mouse) {
                    uiohook_helper::queue.mutex.lock();
                    for (auto const &event : uiohook_helper::queue.events) {
                        have_data = true;
                        buf.write(uint8_t(0)); // uiohook event
                        buf.write(event);
                    }
                    uiohook_helper::queue.events.clear();
                    uiohook_helper::queue.mutex.unlock();
                }

                if (util::cfg.monitor_gamepad) {
                    gamepad_helper::queue.mutex.lock();
                    for (auto const &event : gamepad_helper::queue.events) {
                        have_data = true;
                        buf.write(uint8_t(1)); // sdl event
                        buf.write(event);
                        if (event.type == SDL_CONTROLLERDEVICEADDED) {
                            auto *name = SDL_GameControllerNameForIndex(event.cdevice.which);
                            std::string full_name =
                                name + std::string(" - ") + controller_description(event.cdevice.which);
                            buf.write<uint8_t>(full_name.length());
                            buf.write(full_name.c_str(), full_name.length());
                        }
                    }

                    gamepad_helper::queue.events.clear();
                    gamepad_helper::queue.mutex.unlock();
                }

                if (have_data)
                    mg_ws_send(c, buf.get<const char *>(), buf.write_pos(), WEBSOCKET_OP_BINARY);
            }
        }
    });
    return !!network_thread.native_handle();
}

void stop()
{
    if (!status)
        return;
    status = false;
    connected = false;
    network_thread.join();
    binfo("network stopped");
}
}
