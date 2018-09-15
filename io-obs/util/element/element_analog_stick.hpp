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
    BUTTON_STATE,
    STICK_STATE
};

class element_data_analog_stick : public element_data
{
public:
    /*
        Separate constructors are used on linux
        because the values can't be queried together
    */
    element_data_analog_stick(const button_state state)
        : element_data(BUTTON)
    {
        m_state = state;
        m_data_type = BUTTON_STATE;
    }

    element_data_analog_stick(const float l_x, const float l_y,
                              const float r_x, const float r_y)
        : element_data(BUTTON)
    {
        m_left_stick = {l_x, r_x};
        m_right_stick = {r_x, r_y};
        m_data_type = STICK_STATE;
    }

    element_data_analog_stick(const button_state state, const float l_x,
                              const float l_y,
                              const float r_x, const float r_y)
        : element_data(BUTTON)
    {
        m_left_stick = {l_x, r_x};
        m_right_stick = {r_x, r_y};
        m_state = state;
        m_data_type = BOTH;
    }

    bool pressed() const
    {
        return m_state == STATE_PRESSED;
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
    button_state m_state;
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
private:
    void calc_position(vec2* v, element_data_analog_stick* d) const;
    gs_rect m_pressed;
    element_side m_side;
    uint8_t m_radius = 0;
};
