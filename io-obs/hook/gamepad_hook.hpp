/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2019 univrsal <universailp@web.de>.
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

#include "util/util.hpp"
#ifdef _WIN32
struct js_event;		/* placeholder */
#include "xinput_fix.hpp"
#else
#include "gamepad_binding.hpp"
#include <string>
#include <linux/joystick.h>
#endif /* LINUX */
#include <stdio.h>
#include <mutex>

namespace gamepad
{
    class gamepad_handle {
        int8_t m_id = -1;
        bool m_valid = false;
#ifdef LINUX
		std::string m_path;
		js_event m_event;
		int m_device_id = -1;
#else
		xinput_fix::gamepad m_xinput;
#endif
    public:
        gamepad_handle() {}
        ~gamepad_handle();

        int8_t get_id() const { return m_id; }

        void load();
        void update();
        void unload();
        void init(uint8_t id);
        bool valid() const { return m_valid; }

        /* Linux only */
        int read_event();
        js_event *get_event();
    };

#ifdef _WIN32
    DWORD WINAPI hook_method(LPVOID arg);
#else
    void* hook_method(void*);
#endif

    void start_pad_hook();
    void end_pad_hook();
    bool init_pad_devices();

#ifdef LINUX
    extern gamepad_binding bindings[PAD_COUNT];
#endif

    /* Mutex for thread safety */
    extern std::mutex mutex;
    /* Four structs containing info to query gamepads */
    extern gamepad_handle pads[PAD_COUNT];
    /* Init state of hook */
    extern bool gamepad_hook_state;
    /* False will end thread */
    extern bool gamepad_hook_run_flag;
}
