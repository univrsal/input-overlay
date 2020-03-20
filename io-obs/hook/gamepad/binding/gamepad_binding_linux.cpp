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

#include "gamepad_binding_linux.hpp"

namespace gamepad {

pad_button_events binding_linux::get_button_event_by_id(uint8_t id)
{
        for (const auto &ev : m_button_bindings)
                if (ev.first == id)
                        return ev.second;
        return PAD_BUTTON_INVALID;
}

pad_axis_events binding_linux::get_axis_event_by_id(uint8_t id)
{
        for (const auto &ev : m_axis_bindings)
                if (ev.first == id)
                        return ev.second;
        return PAD_AXIS_INVALID;
}

void binding_linux::init_default()
{
        m_axis_bindings.clear();
        m_button_bindings.clear();
        int i;
        for (i = 0; i < PAD_AXIS_EVENT_COUNT; i++)
                m_axis_bindings[i] = static_cast<pad_axis_events>(i);
        for (i = 0; i < PAD_BUTTON_EVENT_COUNT; i++)
                m_button_bindings[i] = static_cast<pad_button_events>(i);
}

void binding_linux::set_binding(uint8_t id, uint8_t binding, bool axis_event)
{
        if (axis_event) {
                if (binding < PAD_AXIS_EVENT_COUNT)
                        m_axis_bindings[id] = static_cast<pad_axis_events>(binding);
        } else {
                if (binding < PAD_BUTTON_EVENT_COUNT)
                        m_button_bindings[id] = static_cast<pad_button_events>(binding);
        }
}

void gamepad_binding::handle_event(uint8_t pad_id, element_data_holder *data, js_event *event)
{
        uint16_t vc = 0;

        if (event->type == JS_EVENT_BUTTON) {
                auto state = event->value ? BS_PRESSED : BS_RELEASED;
                vc = get_button_event_by_id(event->number);

                if (vc == PAD_L_STICK)
                        data->add_gamepad_data(pad_id, VC_STICK_DATA, new element_data_analog_stick(state, ES_LEFT));
                else if (vc == PAD_R_STICK)
                        data->add_gamepad_data(pad_id, VC_STICK_DATA, new element_data_analog_stick(state, ES_RIGHT));
                else if (vc == PAD_DOWN)
                        data->add_gamepad_data(pad_id, VC_DPAD_DATA, new element_data_dpad(DD_DOWN, state));
                else if (vc == PAD_UP)
                        data->add_gamepad_data(pad_id, VC_DPAD_DATA, new element_data_dpad(DD_UP, state));
                else if (vc == PAD_LEFT)
                        data->add_gamepad_data(pad_id, VC_DPAD_DATA, new element_data_dpad(DD_LEFT, state));
                else if (vc == PAD_RIGHT)
                        data->add_gamepad_data(pad_id, VC_DPAD_DATA, new element_data_dpad(DD_RIGHT, state));

                if (vc != PAD_L_STICK && vc != PAD_R_STICK)
                        data->add_gamepad_data(pad_id, PAD_TO_VC(vc), new element_data_button(state));

        } else if (event->type == JS_EVENT_AXIS) {
                vc = get_axis_event_by_id(event->number);
                float axis;

                if (vc == PAD_LT || vc == PAD_RT) {
                        auto trigger = vc == PAD_LT ? TD_LEFT : TD_RIGHT;
                        /* Trigger data goes from ~ -32000 to +32000, so it's offset by 0x7FFF
                                 * and then divided by 0xffff to convert it to a float (0.0 - 1.0) */
                        axis = (event->value + (0xffff / 2)) / ((float)0xffff);
                        data->add_gamepad_data(pad_id, VC_TRIGGER_DATA, new element_data_trigger(trigger, axis));
                } else {
                        axis = event->value / ((float)0xffff);
                        stick_data_type sd;
                        if (vc == PAD_LY)
                                sd = SD_LEFT_Y;
                        else if (vc == PAD_LX)
                                sd = SD_LEFT_X;
                        else if (vc == PAD_RY)
                                sd = SD_RIGHT_Y;
                        else
                                sd = SD_RIGHT_X;

                        data->add_gamepad_data(pad_id, VC_STICK_DATA, new element_data_analog_stick(axis, sd));
                }
        }
}
}
