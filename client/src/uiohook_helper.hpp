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
#include <uiohook.h>
#include <atomic>
#include <mutex>
#include <buffer.hpp>
#include <vector>

#define SCROLL_TIMEOUT 120
namespace uiohook_helper {
inline uint16_t util_mouse_fix(int m)
{
#ifndef _WIN32 /* Linux mixes right mouse and middle mouse or is windows getting it wrong? */
    if (m == 3)
        m = 2;
    else if (m == 2)
        m = 3;
#endif
    return m;
}

extern std::atomic<bool> hook_state;

struct event_queue {
    std::vector<uiohook_event> events;
    std::mutex mutex;
};

extern event_queue queue;

void dispatch_proc(uiohook_event *event, void *);
bool start();
void stop();

}
