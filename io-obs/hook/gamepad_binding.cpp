/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "util/util.hpp"
#include "util/element/element_analog_stick.hpp"
#include "util/element/element_button.hpp"
#include "util/element/element_dpad.hpp"
#include "util/element/element_trigger.hpp"
#include "util/element/element_data_holder.hpp"
#include "gamepad_binding.hpp"

#define ID_TYPE         6
#define ID_BUTTON       1
#define ID_STATE_1      4
#define ID_STATE_2      5
#define ID_KEY_CODE     7
#define ID_PRESSED      1

#define ID_L_ANALOG_X   0
#define ID_L_ANALOG_Y   1
#define ID_L_TRIGGER    2
#define ID_R_ANALOG_X   3
#define ID_R_ANALOG_Y   4
#define ID_R_TRIGGER    5

namespace gamepad
{
    void gamepad_binding::handle_packet(unsigned char* packet[8], element_data_holder* data, uint8_t pad_id)
    {
        uint16_t vc = 0;
        auto state = (*packet)[ID_STATE_1] == ID_PRESSED ? STATE_PRESSED : STATE_RELEASED;

        if ((*packet)[ID_TYPE] == ID_BUTTON) {
            vc = get_button_event_by_id((*packet)[ID_KEY_CODE]);

            if (vc == EVENT_L_STICK_PRESSED)
                data->add_gamepad_data(pad_id, VC_STICK_DATA, new element_data_analog_stick(state, SIDE_LEFT));
            else if (vc == EVENT_R_STICK_PRESSED)
                data->add_gamepad_data(pad_id, VC_STICK_DATA, new element_data_analog_stick(state, SIDE_RIGHT));
            else if (vc == EVENT_DPAD_DOWN)
                data->add_gamepad_data(pad_id, VC_DPAD_DATA, new element_data_dpad(DPAD_DOWN, state));
            else if (vc == EVENT_DPAD_UP)
                data->add_gamepad_data(pad_id, VC_DPAD_DATA, new element_data_dpad(DPAD_UP, state));
            else if (vc == EVENT_DPAD_LEFT)
                data->add_gamepad_data(pad_id, VC_DPAD_DATA, new element_data_dpad(DPAD_LEFT, state));
            else if (vc == EVENT_DPAD_RIGHT)
                data->add_gamepad_data(pad_id, VC_DPAD_DATA, new element_data_dpad(DPAD_RIGHT, state));
            else
                data->add_gamepad_data(pad_id, vc, new element_data_button(state));
        } else {
            float axis;
            stick_data_type sd;
            vc = get_axis_event_by_id((*packet)[ID_KEY_CODE]);
            auto trigger = vc == EVENT_R_TRIGGER ? T_DATA_RIGHT : T_DATA_LEFT;

            if (vc == EVENT_R_TRIGGER || vc == EVENT_L_TRIGGER) {
                data->add_gamepad_data(pad_id, VC_TRIGGER_DATA,
                                       new element_data_trigger(trigger, (*packet)[ID_STATE_1] / 255.f));
            } else {
                if ((*packet)[ID_STATE_2] < 128)
                    axis = UTIL_CLAMP(-1.f, ((*packet)[ID_STATE_2]) / STICK_MAX_VAL, 1.f);
                else
                    axis = UTIL_CLAMP(-1.f, ((*packet)[ID_STATE_2] - 255) / STICK_MAX_VAL, 1.f);

                if (vc == EVENT_L_ANALOG_Y)
                    sd = SD_LEFT_Y;
                else if (vc == EVENT_L_ANALOG_X)
                    sd = SD_LEFT_X;
                else if (vc == EVENT_R_ANALOG_X)
                    sd = SD_RIGHT_Y;
                else
                    sd = SD_RIGHT_X;

                data->add_gamepad_data(pad_id, VC_STICK_DATA, new element_data_analog_stick(axis, sd));
            }
        }
    }

    pad_events gamepad_binding::get_button_event_by_id(uint8_t id)
    {
        for (const auto &ev : m_button_bindings)
            if (ev.first == id)
                return ev.second;
        return EVENT_INVALID;
    }

    pad_events gamepad_binding::get_axis_event_by_id(uint8_t id)
    {
        for (const auto &ev : m_axis_bindings)
            if (ev.first == id)
                return ev.second;
        return EVENT_INVALID;
    }

    gamepad_binding::gamepad_binding()
    {
        init_default();
    }

    void gamepad_binding::init_default()
    {
        m_axis_bindings.clear();
        m_button_bindings.clear();

        m_axis_bindings[ID_L_TRIGGER] = EVENT_L_TRIGGER;
        m_axis_bindings[ID_R_TRIGGER] = EVENT_R_TRIGGER;
        m_axis_bindings[ID_R_ANALOG_X] = EVENT_R_ANALOG_X;
        m_axis_bindings[ID_R_ANALOG_Y] = EVENT_R_ANALOG_Y;
        m_axis_bindings[ID_L_ANALOG_X] = EVENT_L_ANALOG_X;
        m_axis_bindings[ID_L_ANALOG_Y] = EVENT_L_ANALOG_Y;

        m_button_bindings[PAD_DPAD_DOWN] = EVENT_DPAD_DOWN;
        m_button_bindings[PAD_DPAD_UP] = EVENT_DPAD_UP;
        m_button_bindings[PAD_DPAD_LEFT] = EVENT_DPAD_LEFT;
        m_button_bindings[PAD_DPAD_RIGHT] = EVENT_DPAD_RIGHT;
        m_button_bindings[PAD_L_ANALOG] = EVENT_L_STICK_PRESSED;
        m_button_bindings[PAD_R_ANALOG] = EVENT_R_STICK_PRESSED;

        m_button_bindings[PAD_A] = EVENT_A;
        m_button_bindings[PAD_B] = EVENT_B;
        m_button_bindings[PAD_X] = EVENT_X;
        m_button_bindings[PAD_Y] = EVENT_Y;
        m_button_bindings[PAD_START] = EVENT_START;
        m_button_bindings[PAD_BACK] = EVENT_BACK;
        m_button_bindings[PAD_RB] = EVENT_RB;
        m_button_bindings[PAD_LB] = EVENT_LB;
        m_button_bindings[PAD_GUIDE] = EVENT_GUIDE;
    }
}