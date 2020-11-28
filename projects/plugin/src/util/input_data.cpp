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
    last_mouse_clicked = other->last_mouse_clicked;
    last_mouse_dragged = other->last_mouse_dragged;
    last_mouse_movement = other->last_mouse_movement;
    last_mouse_pressed = other->last_mouse_pressed;
    last_mouse_released = other->last_mouse_released;
    last_axis_event = other->last_axis_event;
    last_button_event = other->last_button_event;
}
