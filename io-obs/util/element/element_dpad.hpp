/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "../layout_constants.hpp"
#include "element_texture.hpp"

class element_data_dpad : public element_data
{
public:
    /*
        Separate constructors are used on linux
        because the values can't be queried together
    */

    /* Xinput directly generates direction */
    element_data_dpad(const dpad_direction a, const dpad_direction b)
        : element_data(DPAD_STICK)
    {
        m_direction = merge_directions(a, b);
    }

    element_data_dpad(const dpad_direction d, const button_state state)
        : element_data(DPAD_STICK)
    {
        m_direction = d;
        m_state = state;
    }

    bool is_persistent() override { return true; }

    void merge(element_data* other) override;

    static dpad_direction merge_directions(dpad_direction a, dpad_direction b);

    dpad_direction get_direction() const { return m_direction; }

    button_state get_state() { return m_state; }
private:
    dpad_direction m_direction = DPAD_CENTER;
    button_state m_state = STATE_PRESSED;
};

class element_dpad : public element_texture
{
public:
    element_dpad() : element_texture(DPAD_STICK)
    {
    };

    void load(ccl_config* cfg, const std::string& id) override;

    void draw(gs_effect_t* effect, gs_image_file_t* image,
        element_data* data, sources::shared_settings* settings) override;

    data_source get_source() override { return GAMEPAD; }

private:
    /* Center is in m_mapping */
    gs_rect m_mappings[8]; /* Left, Right, Up, Down, Top Left, Top Right, Bottom Left, Bottom Right */
};
