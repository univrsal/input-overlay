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

#pragma once
#include "../network/websocket_server.hpp"
#include "../util/config.hpp"
#include <input_data.hpp>
#include <mutex>
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
    static input_data thread_data;
    static const uint64_t refresh_ms = 16;
    static uint64_t last_time = 0;
    auto diff = (event->time - last_time);

    // Mouse move/drag can get very spammy so those events
    // will only be dispatched at 60hz
    const bool is_important = event->type < EVENT_MOUSE_MOVED;

    thread_data.dispatch_uiohook_event(event);
    if (is_important || (diff >= refresh_ms && local_data::data.last_event < thread_data.last_event)) {
        last_time = event->time;
        std::lock_guard<std::mutex> lock(local_data::data.m_mutex);
        local_data::data.copy(&thread_data);
    }
    if (event->type == EVENT_MOUSE_WHEEL)
        last_scroll_time = os_gettime_ns();
    if (!io_config::io_window_filters.input_blocked())
        wss::dispatch_uiohook_event(event, "local");
}

inline uint16_t mouse_fix(int m)
{
    /* Forked uihook should no require this anymore, todo: remove the function*/
    return m;
}

void start();

void stop();

}
