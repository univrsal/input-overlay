/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
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
#include <map>
#include <mutex>
#include <netlib.h>
#include <uiohook.h>

#define SCROLL_TIMEOUT 120

namespace uiohook {
extern std::mutex hook_mutex;
extern uint32_t last_scroll_time;
enum wheel_dir { wheel_up = -1, wheel_none, wheel_down };

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

inline bool is_middle_mouse(int m)
{
    return util_mouse_fix(m) == MOUSE_BUTTON3;
}

extern volatile bool hook_state;
bool logger_proc(unsigned level, const char *format, ...);

void dispatch_proc(uiohook_event *event);
bool start();
void stop();
}