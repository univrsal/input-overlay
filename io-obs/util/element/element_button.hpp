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
    element_data_button(const button_state state) : element_data(ET_BUTTON)
    {
        m_state = state;
        blog(LOG_DEBUG, "state: %i", static_cast<bool>(state));
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
    element_button() : element_texture(ET_BUTTON), m_pressed()
    {
    }

    void load(ccl_config* cfg, const std::string &id) override;

    void
    draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data, sources::overlay_settings* settings) override;

    data_source get_source() override
    { return is_gamepad ? DS_GAMEPAD : DS_DEFAULT; }

private:
    bool is_gamepad = false;
    gs_rect m_pressed;
};
