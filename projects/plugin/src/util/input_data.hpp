/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2021 univrsal <uni@vrsal.de>.
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
#include <uiohook.h>
#include <libgamepad.hpp>

/* Holds all input data for a computer, local or remote */
struct input_data {
    std::mutex m_mutex;
    /* State of all keyboard keys*/
    std::map<uint16_t, bool> keyboard{};

    /* State of all mouse buttons */
    std::map<uint16_t, bool> mouse{};

    /* Last uiohook events */
    keyboard_event_data last_key_pressed{}, last_key_released{}, last_key_typed{};
    mouse_event_data last_mouse_pressed{}, last_mouse_released{}, last_mouse_clicked{}, last_mouse_movement{},
        last_mouse_dragged{};
    mouse_wheel_event_data last_wheel_event{};

    /* Gamepad data */
    std::map<uint16_t, float> gamepad_axis{};
    std::map<uint16_t, bool> gamepad_buttons{};
    gamepad::input_event last_axis_event{};
    gamepad::input_event last_button_event{};

    /* Mutex needs to be locked */
    void copy(const input_data *other);

    void dispatch_uiohook_event(const uiohook_event *event);
};

namespace local_data {
extern std::mutex data_mutex;
extern input_data data;
}
