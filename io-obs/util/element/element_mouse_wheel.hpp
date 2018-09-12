/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include "../layout_constants.hpp"
#include "element.hpp"
#include "element_texture.hpp"

#define WHEEL_MIDDLE  0
#define WHEEL_UP      1
#define WHEEL_DOWN    2

enum wheel_data_type
{
    BUTTON_STATE,
    WHEEL_STATE
};

class element_data_wheel : public element_data
{
public:
    element_data_wheel(const WheelDirection dir, const int amount)
        : element_data(MOUSE_SCROLLWHEEL)
    {
        m_data_type = WHEEL_STATE;
        m_dir = dir;
        m_amount = amount;
    }
    element_data_wheel(const ButtonState state)
        : element_data(MOUSE_SCROLLWHEEL)
    {
        m_data_type = BUTTON_STATE;
        m_middle_button = state;
    }

    int get_amount() const
    {
        return m_amount;
    }

    WheelDirection get_dir() const
    {
        return m_dir;
    }

    void set_dir(const WheelDirection dir)
    {
        m_dir = dir;
    }

    ButtonState get_state() const
    {
        return m_middle_button;
    }

    bool is_presistent() override { return true; }

    void merge(element_data* other) override;
private:
    wheel_data_type m_data_type;
    ButtonState m_middle_button = STATE_RELEASED;
    WheelDirection m_dir;
    int m_amount = 0;
};

class element_wheel : public element_texture
{
public:
    element_wheel() : element_texture(MOUSE_SCROLLWHEEL)
    {
        /* NO-OP */
    };

    void load(ccl_config* cfg, const std::string& id) override;
    void draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data) override;
private:
    /* Middle, Up, Down */
    gs_rect m_mappings[3];
};
