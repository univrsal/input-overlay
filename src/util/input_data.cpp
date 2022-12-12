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

#include "input_data.hpp"

namespace local_data {
input_data data;
}

void input_data::copy(const input_data *other)
{
    keyboard = other->keyboard;
    mouse = other->mouse;
    last_wheel_event = other->last_wheel_event;
}

void input_data::dispatch_uiohook_event(const uiohook_event *event)
{
    if (event->type == EVENT_MOUSE_WHEEL) {
        std::lock_guard<std::mutex> lock(m_mutex);
        last_wheel_event = event->data.wheel;
    } else if (event->type == EVENT_MOUSE_DRAGGED || event->type == EVENT_MOUSE_MOVED) {
        std::lock_guard<std::mutex> lock(m_mutex);
        last_mouse_movement = event->data.mouse;
    }
}
