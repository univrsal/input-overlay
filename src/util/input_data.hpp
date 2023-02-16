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

#include <unordered_map>
#include <atomic>
#include <mutex>
#include <array>
#include <uiohook.h>

#define GAMEPAD_AXIS_MAX 6
#define GAMEPAD_BUTTON_MAX 21

/* Holds all input data for a computer, local or remote */
struct input_data {
    using button_map = std::unordered_map<uint16_t, bool>;

    template<class T, std::size_t L> using gamepad_map = std::unordered_map<int, std::array<T, L>>;

    std::mutex m_mutex;

    std::atomic<uint64_t> last_event = 0;

    /* State of all keyboard keys*/
    button_map keyboard{};

    /* State of all mouse buttons */
    button_map mouse{};

    /* we use this to reset the scroll wheel after a time out */
    mouse_wheel_event_data last_wheel_event{};

    /* used for the mouse motion event */
    mouse_event_data last_mouse_movement{};

    /* Gamepad data */
    gamepad_map<bool, GAMEPAD_BUTTON_MAX> gamepad_buttons;
    gamepad_map<float, GAMEPAD_AXIS_MAX> gamepad_axis;

    /* Mutex needs to be locked */
    void copy(const input_data *other);

    void dispatch_uiohook_event(const uiohook_event *event);
};

namespace local_data {
extern input_data data;
}
