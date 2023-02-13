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

#pragma once
#include "../util/input_data.hpp"
#include "../network/websocket_server.hpp"
#include <mutex>
#include <netlib.h>
#include <uiohook.h>
#include <util/platform.h>
#define SCROLL_TIMEOUT 120000000

namespace uiohook {
extern uint64_t last_scroll_time;
extern bool state;

inline void check_wheel()
{
    if (last_scroll_time && os_gettime_ns() - last_scroll_time >= SCROLL_TIMEOUT) {
        last_scroll_time = 0;
        local_data::data.last_wheel_event = {};
    }
}

inline void process_event(uiohook_event *event)
{
    local_data::data.dispatch_uiohook_event(event);
    if (event->type == EVENT_MOUSE_WHEEL)
        last_scroll_time = os_gettime_ns();
    wss::dispatch_uiohook_event(event, "local");
}

void start();

void stop();

}
