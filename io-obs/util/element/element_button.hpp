/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "../layout_constants.hpp"
#include "element_texture.hpp"

class element_data_button : public element_data
{
public:
    element_data_button(const ButtonState state)
        : element_data(BUTTON)
    {
        m_state = state;
    }

    ButtonState get_state() const
    {
        return m_state;
    }

private:
    ButtonState m_state;
};


class element_button : public element_texture
{
public:
    element_button() : element_texture(BUTTON)
    {
    };
    void load(ccl_config* cfg, const std::string& id) override;
    void draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data) override;
private:
    gs_rect m_pressed;
};
