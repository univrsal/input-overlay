/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "../../sources/input_source.hpp"
#include "element_button.hpp"
#include "../../../ccl/ccl.hpp"

void element_data_button::merge(element_data* other)
{
    if (other && other->get_type() == BUTTON)
    {
        const auto other_btn = dynamic_cast<element_data_button*>(other);
        if (other_btn)
        {
            m_state = other_btn->m_state;
        }
    }
    else
    {
        m_state = STATE_RELEASED;
    }
}

void element_button::load(ccl_config* cfg, const std::string& id)
{
    element_texture::load(cfg, id);
    m_keycode = cfg->get_int(id + CFG_KEY_CODE);
    m_pressed = m_mapping;
    m_pressed.y = m_mapping.y + m_mapping.cy + CFG_INNER_BORDER;
    /* Checks whether first 8 bits are equal */
    is_gamepad = (m_keycode >> 8) == (VC_PAD_MASK >> 8);
}

void element_button::draw(gs_effect_t* effect, gs_image_file_t* image,
    element_data* data, sources::overlay_settings* settings)
{
    if (data)
    {
        const auto button = dynamic_cast<element_data_button*>(data);
        if (button)
        {
            if (button->get_state() == STATE_PRESSED)
            {
                element_texture::draw(effect, image, &m_pressed);
            }
            else
            {
                element_texture::draw(effect, image, nullptr);
            }
        }
    }
    else
    {
        element_texture::draw(effect, image, nullptr);
    }
}
