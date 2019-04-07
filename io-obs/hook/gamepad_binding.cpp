/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include <linux/joystick.h>
#include "util/util.hpp"
#include "util/element/element_analog_stick.hpp"
#include "util/element/element_button.hpp"
#include "util/element/element_dpad.hpp"
#include "util/element/element_trigger.hpp"
#include "util/element/element_data_holder.hpp"
#include "gamepad_binding.hpp"

namespace gamepad
{
    binding default_bindings[21] = {{S_BINDING_A,          "txt_a",          PAD_A},
                                    {S_BINDING_B,          "txt_b",          PAD_B},
                                    {S_BINDING_X,          "txt_x",          PAD_X},
                                    {S_BINDING_Y,          "txt_y",          PAD_Y},
                                    {S_BINDING_GUIDE,      "txt_guide",      PAD_GUIDE},
                                    {S_BINDING_LT,         "txt_lt",         PAD_LT},
                                    {S_BINDING_RT,         "txt_rt",         PAD_RT},
                                    {S_BINDING_RB,         "txt_rb",         PAD_RB},
                                    {S_BINDING_START,      "txt_start",      PAD_START},
                                    {S_BINDING_BACK,       "txt_back",       PAD_BACK},
                                    {S_BINDING_DPAD_UP,    "txt_dpad_up",    PAD_UP},
                                    {S_BINDING_DPAD_DOWN,  "txt_dpad_down",  PAD_DOWN},
                                    {S_BINDING_DPAD_LEFT,  "txt_dpad_left",  PAD_LEFT},
                                    {S_BINDING_LB,         "txt_lb",         PAD_LB},
                                    {S_BINDING_DPAD_RIGHT, "txt_dpad_right", PAD_RIGHT},
                                    {S_BINDING_ANALOG_L,   "txt_analog_l",   PAD_L_STICK},
                                    {S_BINDING_ANALOG_R,   "txt_analog_r",   PAD_R_STICK},
                                    {S_BINDING_ANALOG_LX,  "txt_lx",         PAD_LX},
                                    {S_BINDING_ANALOG_LY,  "txt_ly",         PAD_LX},
                                    {S_BINDING_ANALOG_RX,  "txt_rx",         PAD_RX},
                                    {S_BINDING_ANALOG_RY,  "txt_ry",         PAD_RY},};

    void gamepad_binding::handle_event(uint8_t pad_id, struct js_event* event, element_data_holder* data)
    {
        uint16_t vc = 0;
        auto state = static_cast<button_state>(event->value);

        if (event->type == JS_EVENT_BUTTON) {
            vc = get_button_event_by_id(event->number);
            if (vc == PAD_L_STICK)
                data->add_gamepad_data(pad_id, VC_STICK_DATA, new element_data_analog_stick(state, SIDE_LEFT));
            else if (vc == PAD_R_STICK)
                data->add_gamepad_data(pad_id, VC_STICK_DATA, new element_data_analog_stick(state, SIDE_RIGHT));
            else if (vc == PAD_DOWN)
                data->add_gamepad_data(pad_id, VC_DPAD_DATA, new element_data_dpad(DPAD_DOWN, state));
            else if (vc == PAD_UP)
                data->add_gamepad_data(pad_id, VC_DPAD_DATA, new element_data_dpad(DPAD_UP, state));
            else if (vc == PAD_LEFT)
                data->add_gamepad_data(pad_id, VC_DPAD_DATA, new element_data_dpad(DPAD_LEFT, state));
            else if (vc == PAD_RIGHT)
                data->add_gamepad_data(pad_id, VC_DPAD_DATA, new element_data_dpad(DPAD_RIGHT, state));
            else
                data->add_gamepad_data(pad_id, PAD_TO_VC(vc), new element_data_button(state));
        } else if (event->type == JS_EVENT_AXIS) {
            vc = get_axis_event_by_id(event->number);
            float axis = 0.f;

            if (vc == PAD_LT || vc == PAD_RT) {
                auto trigger = vc == PAD_LT ? T_DATA_LEFT : T_DATA_RIGHT;
                axis = event->value + (0xffff / 2) / ((float) 0xffff);
                data->add_gamepad_data(pad_id, VC_TRIGGER_DATA, new element_data_trigger(trigger, axis));
            } else {
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

    pad_button_events gamepad_binding::get_button_event_by_id(uint8_t id)
    {
        for (const auto &ev : m_button_bindings)
            if (ev.first == id)
                return ev.second;
        return PAD_BUTTON_INVALID;
    }

    pad_axis_events gamepad_binding::get_axis_event_by_id(uint8_t id)
    {
        for (const auto &ev : m_axis_bindings)
            if (ev.first == id)
                return ev.second;
        return PAD_AXIS_INVALID;
    }

    gamepad_binding::gamepad_binding()
    {
        init_default();
    }

    void gamepad_binding::init_default()
    {
        m_axis_bindings.clear();
        m_button_bindings.clear();
        int i;
        for (i = 0; i < PAD_AXIS_EVENT_COUNT; i++)
            m_axis_bindings[i] = static_cast<pad_axis_events>(i);
        for (i = 0; i < PAD_BUTTON_EVENT_COUNT; i++)
            m_button_bindings[i] = static_cast<pad_button_events>(i);
    }
}