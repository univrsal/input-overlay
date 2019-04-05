/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <map>

/* On linux the gamepad inputs are read from /dev/js*
 * since this only provides direct input events some
 * gamepads might report different events for the same
 * keypresses, so this class loads configurations which
 * map a /dev/js* event to a button identifiable by the plugin
 */

class element_data_holder;

namespace gamepad
{
    enum pad_events
    {
        EVENT_INVALID = -1,
        /* Special buton events */
        EVENT_L_STICK_PRESSED,
        EVENT_R_STICK_PRESSED,
        EVENT_DPAD_DOWN,
        EVENT_DPAD_UP,
        EVENT_DPAD_LEFT,
        EVENT_DPAD_RIGHT,
        /* Axis events */
        EVENT_L_TRIGGER,
        EVENT_R_TRIGGER,
        EVENT_R_ANALOG_X,
        EVENT_R_ANALOG_Y,
        EVENT_L_ANALOG_X,
        EVENT_L_ANALOG_Y,

        /* Normal button events */
        EVENT_A = PAD_A | VC_PAD_MASK,
        EVENT_B = PAD_B | VC_PAD_MASK,
        EVENT_X = PAD_X | VC_PAD_MASK,
        EVENT_Y = PAD_Y | VC_PAD_MASK,
        EVENT_START = PAD_START | VC_PAD_MASK,
        EVENT_BACK = PAD_BACK | VC_PAD_MASK,
        EVENT_RB = PAD_RB | VC_PAD_MASK,
        EVENT_LB = PAD_LB | VC_PAD_MASK,
        EVENT_GUIDE = PAD_GUIDE | VC_PAD_MASK
    };

    class gamepad_binding
    {
        std::map<uint8_t, pad_events> m_axis_bindings;
        std::map<uint8_t, pad_events> m_button_bindings;

        pad_events get_button_event_by_id(uint8_t id);

        pad_events get_axis_event_by_id(uint8_t id);

    public:
        gamepad_binding();

        void init_default();

        void load_from_file(const char* path);

        void handle_packet(unsigned char* packet[8], element_data_holder* data, uint8_t pad_id);
    };
}