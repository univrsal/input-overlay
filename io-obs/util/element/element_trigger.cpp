/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "../../ccl/ccl.hpp"
#include "element_trigger.hpp"
#include "../util.hpp"

void element_trigger::load(ccl_config* cfg, const std::string& id, const vec2* default_size)
{
    element_texture::load(cfg, id, default_size);
    m_button_mode = cfg->get_bool(id + CFG_TRIGGER_MODE);
    m_side = static_cast<element_side>(cfg->get_int(id + CFG_SIDE));
    m_keycode = VC_TRIGGER_DATA;
    m_pressed = m_mapping;
    m_pressed.y = m_mapping.y + m_mapping.cy + CFG_OUTER_BORDER;
    if (!m_button_mode)
    {
        m_direction = static_cast<trigger_direction>(cfg->get_int(
            id + CFG_DIRECTION));
    }
}

void element_trigger::draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data)
{
    if (data)
    {
        const auto trigger = dynamic_cast<element_data_trigger*>(data);
        auto progress = 0.f;
        if (trigger)
        {
            switch(m_side)
            {
            case SIDE_LEFT:
                progress = trigger->get_left();
                break;
            case SIDE_RIGHT:
                progress = trigger->get_right();
                break;
            default: ;
            }
            
            if (m_button_mode)
            {
                if(progress >= 0.1)
                {
                    element_texture::draw(effect, image, &m_pressed);
                }
                else
                {
                    element_texture::draw(effect, image, &m_mapping);
                }  
            }
            else
            {
                auto crop = m_pressed;
                auto new_pos = m_pos;
                calculate_mapping(&crop, &new_pos, progress);
                element_texture::draw(effect, image, &m_mapping); /* Draw unpressed first */
                element_texture::draw(effect, image, &crop, &new_pos);
            }        
        }
    }
    else
    {
         element_texture::draw(effect, image, nullptr);
    }
}

void element_trigger::calculate_mapping(gs_rect * pressed, vec2 * pos, const float progress) const
{
    switch (m_direction)
    {
    case TRIGGER_UP:
        pressed->cy = m_mapping.cy * progress;
        pressed->y = m_pressed.y + (m_mapping.cy - pressed->cy);
        pos->y += m_mapping.cy - pressed->cy;
        break;
    case TRIGGER_DOWN:
        pressed->cy = m_mapping.cy * progress;
        break;
    case TRIGGER_LEFT:
        pressed->cx = m_mapping.cx * progress;
        pressed->x = m_mapping.x + (m_mapping.cx - pressed->cx);
        pos->x += (m_mapping.cx - pressed->cx);
        break;
    case TRIGGER_RIGHT:
        pressed->cx = m_mapping.cx * progress;
        break;
    default: ;
    }
}

void element_data_trigger::merge(element_data* other)
{
    if (other && other->get_type() == m_type)
    {
        const auto trigger = dynamic_cast<element_data_trigger*>(other);

        if (trigger)
        {
            switch (trigger->m_data_type)
            {
            case T_DATA_BOTH:
                m_left_trigger = trigger->m_left_trigger;
                m_right_trigger = trigger->m_right_trigger;
                break;
            case T_DATA_LEFT:
                m_left_trigger = trigger->m_left_trigger;
                break;
            case T_DATA_RIGHT:
                m_right_trigger = trigger->m_right_trigger;
                break;
            default: ;
            }         
        }
    }
}
