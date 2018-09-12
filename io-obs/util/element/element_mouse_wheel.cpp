/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "element_mouse_wheel.hpp"

void element_wheel::load(ccl_config * cfg, const std::string & id)
{
    element_texture::load(cfg, id);

    auto i = 1;
    for (auto map : m_mappings)
    {
        map = m_mapping;
        map.x += i * (m_mapping.cx + CFG_INNER_BORDER);
        i++;
    }
}

void element_wheel::draw(gs_effect_t * effect, gs_image_file_t * image, element_data * data)
{
    if (data)
    {
        const auto wheel = reinterpret_cast<element_data_wheel*>(data);

        if (wheel->get_state() == STATE_PRESSED)
            element_texture::draw(effect, image, &m_mappings[WHEEL_MIDDLE]);

        switch(wheel->get_dir())
        {
        case WHEEL_DIR_UP:
            element_texture::draw(effect, image, &m_mappings[WHEEL_UP]);
            break;
        case WHEEL_DIR_DOWN:
            element_texture::draw(effect, image, &m_mappings[WHEEL_DOWN]);
            break;
        default:
        case WHEEL_DIR_NONE: ;
        }
    }

    element_texture::draw(effect, image, data);
}

void element_data_wheel::merge(element_data * other)
{
    if (other && other->get_type() == m_type)
    {
        const auto other_wheel = reinterpret_cast<element_data_wheel*>(other);
        
        switch(other_wheel->m_data_type)
        {
        case BUTTON_STATE:
            m_middle_button = other_wheel->get_state();
            break;
        case WHEEL_STATE:
            m_amount = other_wheel->get_amount();
            m_dir = other_wheel->get_dir();
            break;
        default: ;
        }
    }
}
