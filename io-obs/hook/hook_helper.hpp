/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <universailp@web.de>.
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

#include <mutex>
#include <uiohook.h>

#ifdef LINUX
#include <stdint.h>
#else
#include <Windows.h>
#endif

class element_data_holder;

namespace hook {
extern element_data_holder* input_data;

extern uint64_t last_wheel;
extern wint_t last_character;
extern int16_t mouse_x, mouse_y, mouse_x_smooth, mouse_y_smooth, mouse_last_x, mouse_last_y;
extern bool hook_initialized;
extern bool data_initialized;
extern std::mutex mutex;

#ifdef _WIN32
DWORD WINAPI hook_thread_proc(LPVOID arg);
#else

void* hook_thread_proc(void* arg);

#endif

/* Checks if mouse scrolling timed out */
void check_wheel();

void dispatch_proc(uiohook_event* event);

bool logger_proc(unsigned int level, const char* format, ...);

void init_data_holder();

void start_hook();

void end_hook();

int hook_enable();

void process_event(uiohook_event* event);
};
