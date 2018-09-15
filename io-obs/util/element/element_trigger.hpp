/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "../layout_constants.hpp"
#include "element_texture.hpp"

enum trigger_data_type
{
    BOTH,
    LEFT,
    RIGHT
};

class element_data_trigger : public element_data
{
public:
    /*
        Separate constructors are used on linux
        because the values can't be queried together
    */
    element_data_trigger(const float left)
        : element_data(TRIGGER)
    {
        m_left_trigger = left;
        m_data_type = LEFT;
    }

    element_data_trigger(const float right)
        : element_data(TRIGGER)
    {
        m_right_trigger = right;
        m_data_type = RIGHT;
    }

    element_data_trigger(const float left, const float right)
        : element_data(TRIGGER)
    {
        m_left_trigger = left;
        m_right_trigger = right;
        m_data_type = BOTH;
    }

    float get_left()
    {
        return m_left_trigger;
    }

    float get_right()
    {
        return m_right_trigger;
    }

    bool is_presistent() override { return true; }

    void merge(element_data* other) override;

private:
    trigger_data_type m_data_type;
    float m_left_trigger = 0.f, m_right_trigger = 0.f;
};

class element_trigger : public element_texture
{
public:
    element_trigger() : element_texture(BUTTON)
    {
    };
    void load(ccl_config* cfg, const std::string& id) override;
    void draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data) override;
private:
    gs_rect m_pressed;
    element_side m_side;
    trigger_direction m_direction;
    bool m_button_mode = false;
};
