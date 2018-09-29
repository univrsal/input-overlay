/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "../../sources/input_source.hpp"
#include "element_analog_stick.hpp"
#include "../../ccl/ccl.hpp"
#include "../util.hpp"

void element_analog_stick::load(ccl_config* cfg, const std::string& id)
{
    element_texture::load(cfg, id);
    m_side = static_cast<element_side>(cfg->get_int(id + CFG_SIDE));
    m_radius = cfg->get_int(id + CFG_STICK_RADIUS);
    m_keycode = VC_STICK_DATA;
    m_pressed = m_mapping;
    m_pressed.y = m_mapping.y + m_mapping.cy + CFG_INNER_BORDER;
}

void element_analog_stick::draw(gs_effect_t* effect, gs_image_file_t* image,
                                element_data* data, sources::shared_settings* settings)
{
    if (data)
    {
        const auto stick = dynamic_cast<element_data_analog_stick*>(data);
        if (stick)
        {
            auto pos = m_pos;
            gs_rect* temp = nullptr;

            if (m_side == SIDE_LEFT)
                temp = stick->left_pressed() ? &m_pressed : &m_mapping;
            else
                temp = stick->right_pressed() ? &m_pressed : &m_mapping;
            calc_position(&pos, stick);
            element_texture::draw(effect, image, temp, &pos);           
        }
    }
    else
    {
        element_texture::draw(effect, image, nullptr);
    }
}

void element_analog_stick::calc_position(
    vec2* v, element_data_analog_stick* d) const
{
    switch (m_side)
    {
    case SIDE_LEFT:
        v->x += d->get_left_stick()->x * m_radius;
        v->y += d->get_left_stick()->y * m_radius;
        break;
    case SIDE_RIGHT:
        v->x += d->get_right_stick()->x * m_radius;
        v->y += d->get_right_stick()->y * m_radius;
        break;
    default: ;
    }
}

void element_data_analog_stick::merge(element_data* other)
{
    if (other && other->get_type() == m_type)
    {
        const auto other_stick
            = dynamic_cast<element_data_analog_stick*>(other);
        if (other_stick)
        {
            switch (other_stick->m_data_type)
            {
            case BOTH:
                m_left_stick = other_stick->m_left_stick;
                m_right_stick = other_stick->m_right_stick;
                m_left_state = other_stick->m_left_state;
                m_right_state = other_stick->m_right_state;
                break;
            case PRESSED_STATE_LEFT:
                m_left_state = other_stick->m_left_state;
                break;
            case PRESSED_STATE_RIGHT:
                m_right_state = other_stick->m_right_state;
                break;
            case STICK_STATE_LEFT_X:
                m_left_stick.x = other_stick->m_left_stick.x;
                break;
            case STICK_STATE_LEFT_Y:
                m_left_stick.y = other_stick->m_left_stick.y;
                break;
            case STICK_STATE_RIGHT_X:
                m_right_stick.x = other_stick->m_right_stick.x;
                break;
            case STICK_STATE_RIGHT_Y:
                m_right_stick.y = other_stick->m_right_stick.y;
                break;
            default: ;
            }            
        }

    }
}
