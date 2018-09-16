/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "element_button.hpp"
#include "../../ccl/ccl.hpp"

void element_button::load(ccl_config* cfg, const std::string& id,
                          const vec2* default_size)
{
    element_texture::load(cfg, id, default_size);
    m_keycode = cfg->get_int(id + CFG_KEY_CODE);
    m_pressed = m_mapping;
    m_pressed.y = m_mapping.y + m_mapping.cy + CFG_OUTER_BORDER;
    is_gamepad = m_keycode & VC_PAD_MASK;
}

void element_button::draw(gs_effect_t* effect, gs_image_file_t* image,
                          element_data* data)
{
    if (data && reinterpret_cast<element_data_button*>(data)->get_state() ==
        STATE_PRESSED)
    {
        element_texture::draw(effect, image, &m_pressed);
    }
    else
    {
        element_texture::draw(effect, image, nullptr);
    }
}
