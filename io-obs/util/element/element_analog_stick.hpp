/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "../layout_constants.hpp"
#include "element_texture.hpp"

enum stick_data_type
{
    BOTH,
    PRESSED_STATE_LEFT,
    PRESSED_STATE_RIGHT,
    STICK_STATE_LEFT_X,
    STICK_STATE_LEFT_Y,
    STICK_STATE_RIGHT_X,
    STICK_STATE_RIGHT_Y
};

/* Contains data for both analog sticks
 */
class element_data_analog_stick : public element_data
{
public:
    /*
        Separate constructors are used on linux
        because the values can't be queried together
    */
    element_data_analog_stick(const button_state state, const element_side side)
        : element_data(BUTTON)
    {
        m_left_state = state;
        m_data_type = side == SIDE_LEFT ? PRESSED_STATE_LEFT : PRESSED_STATE_RIGHT;
    }

    element_data_analog_stick(const float axis_value, const stick_data_type data_type)
        : element_data(BUTTON)
    {
        switch(data_type)
        {
        case STICK_STATE_LEFT_X:
             m_left_stick = { axis_value, -1};
            break;
        case STICK_STATE_LEFT_Y:
            m_left_stick = { -1, axis_value};
            break;
        case STICK_STATE_RIGHT_X:
             m_right_stick = { axis_value, -1};
            break;
        case STICK_STATE_RIGHT_Y:
            m_right_stick = { -1, axis_value};
            break;
        default: ;
        }
        m_data_type = data_type;
    }

    element_data_analog_stick(const button_state left, const button_state right,
                              const float l_x, const float l_y,
                              const float r_x, const float r_y)
        : element_data(BUTTON)
    {
        m_left_stick = {l_x, l_y};
        m_right_stick = {r_x, r_y};
        m_left_state = left;
        m_right_state = right;
        m_data_type = BOTH;
    }

    bool left_pressed() const
    {
        return m_left_state == STATE_PRESSED;
    }

    bool right_pressed() const
    {
        return m_right_state == STATE_PRESSED;
    }

    const vec2* get_left_stick() const
    {
        return &m_left_stick;
    }

    const vec2* get_right_stick() const
    {
        return &m_right_stick;
    }

    bool is_presistent() override { return true; }

    void merge(element_data* other) override;

private:
    vec2 m_left_stick, m_right_stick;
    stick_data_type m_data_type;
    button_state m_left_state, m_right_state;
};

class element_analog_stick : public element_texture
{
public:
    element_analog_stick() : element_texture(BUTTON)
    {
    };
    
    void load(ccl_config* cfg, const std::string& id,
              const vec2* default_size) override;
    
    void draw(gs_effect_t* effect, gs_image_file_t* image,
              element_data* data) override;

    data_source get_source() override { return GAMEPAD; }
private:
    void calc_position(vec2* v, element_data_analog_stick* d) const;
    gs_rect m_pressed;
    element_side m_side;
    uint8_t m_radius = 0;
};
