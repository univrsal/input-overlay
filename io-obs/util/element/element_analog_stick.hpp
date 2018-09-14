/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "../layout_constants.hpp"
#include "element_texture.hpp"

class element_data_analog_stick : public element_data
{
public:
    element_data_analog_stick(const button_state state)
        : element_data(BUTTON)
    {
        m_state = state;
    }

    button_state get_state() const
    {
        return m_state;
    }

private:
    button_state m_state;
};


class element_analog_stick : public element_texture
{
public:
    element_analog_stick() : element_texture(BUTTON)
    {
    };
    void load(ccl_config* cfg, const std::string& id) override;
    void draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data) override;
private:
    gs_rect m_pressed;
    element_side m_side;
};
