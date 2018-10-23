/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "../layout_constants.hpp"
#include "element_texture.hpp"
#include <netlib.h>

class element_data_button : public element_data
{
public:
    element_data_button(const button_state state)
        : element_data(BUTTON)
    {
        m_state = state;
    }

    button_state get_state() const
    {
        return m_state;
    }
	static element_data_button* from_buffer(netlib_byte_buf* buffer);
private:
    button_state m_state;
};

class element_button : public element_texture
{
public:
    element_button() : element_texture(BUTTON), m_pressed()
    {
    }

    void load(ccl_config* cfg, const std::string& id) override;
    void draw(gs_effect_t* effect, gs_image_file_t* image,
        element_data* data, sources::shared_settings* settings) override;

    data_source get_source() override { return is_gamepad ? GAMEPAD : DEFAULT; }
private:
    bool is_gamepad = false;
    gs_rect m_pressed;
};
