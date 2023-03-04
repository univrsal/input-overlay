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
#include <buffer.hpp>
#include <keycodes.h>

#define GAMEPAD_AXIS_MAX 6
#define GAMEPAD_BUTTON_MAX 21

class QJsonObject;

/* Holds all input data for a computer, local or remote */
struct input_data {
    template<class T>
    using button_map = std::unordered_map<T, bool>;

    template<class T, std::size_t L> using gamepad_map = std::unordered_map<int, std::array<T, L>>;

    std::mutex m_mutex;

    std::atomic<uint64_t> last_event = 0;
    std::atomic<uint64_t> last_event_type = 0;

    /* State of all keyboard keys*/
    button_map<uint16_t> keyboard{};

    /* State of all mouse buttons */
    button_map<uint8_t> mouse{};

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

    inline void deserialize(buffer& buf)
    {
        auto count = *buf.read<uint8_t>();
        keyboard.clear();
        for (uint8_t i = 0; i < count; i++) {
            auto keycode = *buf.read<uint16_t>();
            keyboard[keycode] = true;
        }

        mouse.clear();
        count = *buf.read<uint8_t>();
        for (uint8_t i = 0; i < count; i++) {
            auto keycode = *buf.read<uint8_t>();
            mouse[keycode] = true;
        }


        last_wheel_event.direction = *buf.read<uint8_t>();
        last_wheel_event.type = *buf.read<uint8_t>();
        last_wheel_event.amount = *buf.read<uint16_t>();
        last_wheel_event.direction = *buf.read<uint16_t>();

        last_mouse_movement = *buf.read<mouse_event_data>();

        count = *buf.read<uint8_t>(); // number of gamepads (no id/name for now)
        gamepad_buttons.clear();
        for (uint8_t i = 0; i < count; i++) {
            auto button_count = *buf.read<uint8_t>();
            for (uint8_t j = 0; i < button_count; i++) {
                auto button_id = *buf.read<uint8_t>();
                gamepad_buttons[i][button_id] = true;
            }
        }

        gamepad_axis.clear();
        for (uint8_t i = 0; i < count; i++) {
            for (uint8_t j = 0; j < GAMEPAD_AXIS_MAX; j++) {
                auto axis_value = *buf.read<float>();
                gamepad_axis[i][j] = axis_value;
            }
        }
    }

    inline void serialize(buffer& buf)
    {
        auto* count = buf.write<uint8_t>(0);
        for (auto const& key : keyboard) {
            if (key.second) {
                buf.write(key.first);
                *count += 1;
            }
        }

        count = buf.write<uint8_t>(0);
        for (auto const& button : mouse) {
            if (button.second) {
                buf.write(button.first);
                *count += 1;
            }
        }

        buf.write(last_wheel_event.direction);
        buf.write(last_wheel_event.type);
        buf.write(last_wheel_event.amount);
        buf.write(last_wheel_event.rotation);

        buf.write(last_mouse_movement);

        buf.write<uint8_t>(gamepad_buttons.size()); // number of gamepads (no id/name for now)
        for (auto const& buttons : gamepad_buttons) {
            auto* num_pressed_buttons = buf.write<uint8_t>(0);
            uint8_t index = 0;
            // Only send the index of the buttons that are pressed
            for (auto const &button : buttons.second)
            {
                if (button) {
                    buf.write(index);
                    *num_pressed_buttons += 1;
                }
                index++;
            }
        }

        // TODO: This map *should* have the same size so this is redundant
        // buf.write<uint8_t>(gamepad_axis.size());
        for (auto const& axis_array : gamepad_axis) {
            for (auto const &axis : axis_array.second)
                buf.write(axis);
        }
    }
};

namespace local_data {
extern input_data data;
}
