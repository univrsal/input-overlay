/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "element_texture.hpp"

enum dpad_direction;

enum button_state;

class element_data_dpad : public element_data
{
public:
    /*
        Separate constructors are used on linux
        because the values can't be queried together
    */

    /* Xinput directly generates direction */
    element_data_dpad(dpad_direction a, dpad_direction b);

    element_data_dpad(dpad_direction d, button_state state);

    bool is_persistent() override;

    void merge(element_data* other) override;

    static dpad_direction merge_directions(dpad_direction a, dpad_direction b);

    dpad_direction get_direction() const;

    button_state get_state() const;

private:
    dpad_direction m_direction;
    button_state m_state;
};

class element_dpad : public element_texture
{
public:
    element_dpad();

    void load(ccl_config* cfg, const std::string &id) override;

    void
    draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data, sources::overlay_settings* settings) override;

    data_source get_source() override;

private:
    /* Center is in m_mapping */
    gs_rect m_mappings[8]; /* Left, Right, Up, Down, Top Left, Top Right, Bottom Left, Bottom Right */
};
