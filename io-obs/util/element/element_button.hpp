/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "../layout_constants.hpp"
#include "element_texture.hpp"
#include <netlib.h>

class element_data_button : public element_data
{
public:
    element_data_button(const button_state state) : element_data(element_type::BUTTON)
    {
        m_state = state;
    }

    button_state get_state() const
    {
        return m_state;
    }

    bool merge(element_data* other) override;

private:
    button_state m_state;
};

class element_button : public element_texture
{
public:
    element_button() : element_texture(element_type::BUTTON), m_pressed()
    {
    }

    void load(ccl_config* cfg, const std::string &id) override;

    void
    draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data, sources::overlay_settings* settings) override;

    data_source get_source() override
    { return is_gamepad ? data_source::GAMEPAD : data_source::DEFAULT; }

private:
    bool is_gamepad = false;
    gs_rect m_pressed;
};
