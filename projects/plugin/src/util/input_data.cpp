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

#include "input_data.hpp"

namespace local_data {
std::mutex data_mutex;
input_data data;
}

void input_data::copy(const input_data *other)
{
    keyboard = other->keyboard;
    mouse = other->mouse;
    gamepad_buttons = other->gamepad_buttons;
    gamepad_axis = other->gamepad_axis;
    last_key_pressed = other->last_key_pressed;
    last_key_released = other->last_key_released;
    last_key_typed = other->last_key_typed;
    last_mouse_pressed = other->last_mouse_pressed;
    last_mouse_dragged = other->last_mouse_dragged;
    last_mouse_movement = other->last_mouse_movement;
    last_mouse_pressed = other->last_mouse_pressed;
    last_mouse_released = other->last_mouse_released;
    last_axis_event = other->last_axis_event;
    last_button_event = other->last_button_event;
}

void input_data::dispatch_uiohook_event(const uiohook_event *event)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    switch (event->type) {
    case EVENT_KEY_PRESSED:
        last_key_pressed = event->data.keyboard;
        keyboard[event->data.keyboard.keycode] = true;
        break;
    case EVENT_KEY_RELEASED:
        last_key_released = event->data.keyboard;
        keyboard[event->data.keyboard.keycode] = false;
        break;
    case EVENT_KEY_TYPED:
        last_key_typed = event->data.keyboard;
        break;
    case EVENT_MOUSE_WHEEL:
        last_wheel_event = event->data.wheel;
        break;
    case EVENT_MOUSE_PRESSED:
        last_mouse_pressed = event->data.mouse;
        mouse[event->data.mouse.button] = true;
        break;
    case EVENT_MOUSE_RELEASED:
        last_mouse_released = event->data.mouse;
        mouse[event->data.mouse.button] = false;
        break;
    case EVENT_MOUSE_CLICKED:
        last_mouse_clicked = event->data.mouse;
        break;
    case EVENT_MOUSE_DRAGGED:
        last_mouse_dragged = event->data.mouse;
        break;
    case EVENT_MOUSE_MOVED:
        last_mouse_movement = event->data.mouse;
    default:;
    }
}
