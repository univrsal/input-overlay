/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
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

struct js_event;

namespace gamepad
{
    struct binding
    {
        const char* setting;
        const char* text_box_id;
        uint8_t default_value;
        bool axis_event; /* true if axis event */
    };

    extern binding default_bindings[21];

    enum pad_axis_events
    {
        PAD_AXIS_INVALID = -1, PAD_LX, PAD_LY, PAD_LT, PAD_RX, PAD_RY, PAD_RT, PAD_AXIS_EVENT_COUNT
    };

    enum pad_button_events
    {
        PAD_BUTTON_INVALID = -1,
        PAD_A,
        PAD_B,
        PAD_X,
        PAD_Y,
        PAD_LB,
        PAD_RB,
        PAD_BACK,
        PAD_START,
        PAD_GUIDE,
        PAD_L_STICK,
        PAD_R_STICK,
        PAD_LEFT,
        PAD_RIGHT,
        PAD_UP,
        PAD_DOWN,
        PAD_BUTTON_EVENT_COUNT
    };

    class gamepad_binding
    {
        std::map<uint8_t, pad_axis_events> m_axis_bindings;
        std::map<uint8_t, pad_button_events> m_button_bindings;

        pad_button_events get_button_event_by_id(uint8_t id);

        pad_axis_events get_axis_event_by_id(uint8_t id);

    public:
        gamepad_binding();

        void init_default();

        void set_binding(uint8_t id, uint8_t binding, bool axis_event);

        void handle_event(uint8_t pad_id, element_data_holder* data, js_event* event);
    };
}
