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
#ifdef LINUX
#include <linux/joystick.h>
#include <string>
#else

#endif
#include "gamepad_state.hpp"
#include "xinput_fix.hpp"
#include <mutex>
#include <stdint.h>

#define PAD_COUNT 4

namespace gamepad {
enum pad_code {
    CODE_DPAD_UP = 0x0001,
    CODE_DPAD_DOWN = 0x0002,
    CODE_DPAD_LEFT = 0x0004,
    CODE_DPAD_RIGHT = 0x0008,
    CODE_START = 0x0010,
    CODE_BACK = 0x0020,
    CODE_LEFT_THUMB = 0x0040,
    CODE_RIGHT_THUMB = 0x0080,
    CODE_LEFT_SHOULDER = 0x0100,
    CODE_RIGHT_SHOULDER = 0x0200,
    CODE_GUIDE = 0x0400,
    CODE_A = 0x1000,
    CODE_B = 0x2000,
    CODE_X = 0x4000,
    CODE_Y = 0x8000
};

static pad_code pad_keys[] = {
    /* These keycodes are only used on windows,
		 but the linux client converts them to these
		 to agree on one standard
       */
    CODE_A,
    CODE_B,
    CODE_X,
    CODE_Y,
    CODE_GUIDE,
    CODE_DPAD_DOWN,
    CODE_DPAD_UP,
    CODE_DPAD_LEFT,
    CODE_DPAD_RIGHT,
    CODE_LEFT_SHOULDER,
    CODE_RIGHT_SHOULDER,
    CODE_START,
    CODE_BACK
};
#ifndef _WIN32
#define ID_TYPE 6
#define ID_BUTTON 1
#define ID_STATE_1 4
#define ID_STATE_2 5
#define ID_KEY_CODE 7
#define ID_PRESSED 1

#define ID_L_ANALOG_X 0
#define ID_L_ANALOG_Y 1
#define ID_L_TRIGGER 2
#define ID_R_ANALOG_X 3
#define ID_R_ANALOG_Y 4
#define ID_R_TRIGGER 5
#endif /* !WINDOWS */

class gamepad_handle {
    int8_t m_id = -1;
    bool m_valid = false;
    bool m_changed = false;
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
    void mark_dirty() { m_changed = true; }
    void reset() { m_changed = false; }
    bool valid() const { return m_valid; }
    bool changed() const { return m_changed; }
    /* Linux only */
    int read_event();
    js_event* get_event();
};

/* Thread stuff*/
bool start_pad_hook(bool threaded);
bool init_pad_devices();
void end_pad_hook();

bool check_changes();

#ifdef _WIN32
DWORD WINAPI hook_method(LPVOID arg);
#else
void* hook_method(void*);
#endif

#ifdef _WIN32
extern HANDLE hook_thread;
#else
extern pthread_t thread;
#endif
extern gamepad_handle pads[PAD_COUNT];
extern volatile bool state;
extern volatile bool run_flag;

} /* namespace gamepad */
