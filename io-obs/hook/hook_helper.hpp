/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <uiohook.h>
#include <mutex>

#include "../util/util.hpp"

#ifdef LINUX
#include <stdint.h>
#else
#include <Windows.h>
#endif

class element_data_holder;

namespace hook
{
    extern element_data_holder* input_data;

    extern uint64_t last_wheel;
    extern wint_t last_character;
    extern int16_t mouse_x, mouse_y, mouse_x_smooth, mouse_y_smooth, mouse_last_x,
                   mouse_last_y;
    extern bool hook_initialized;
	extern bool data_initialized;
    extern std::mutex mutex;

#ifdef _WIN32
    DWORD WINAPI hook_thread_proc(LPVOID arg);
#else
	void *hook_thread_proc(void *arg);
#endif

    void dispatch_proc(uiohook_event* event);

    bool logger_proc(unsigned int level, const char* format, ...);

	void init_data_holder();

    void start_hook();

    void end_hook();

    int hook_enable();

    void process_event(uiohook_event* event);
};
