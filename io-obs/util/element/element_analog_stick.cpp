/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "element_analog_stick.hpp"
#include "../../ccl/ccl.hpp"
#include "../util.hpp"

void element_analog_stick::load(ccl_config* cfg, const std::string& id)
{
    element_texture::load(cfg, id);
    m_side = (element_side) cfg->get_int(id + CFG_SIDE);
    m_radius = cfg->get_int(id + CFG_STICK_RADIUS);
    m_keycode = VC_STICK_DATA;
    m_pressed = m_mapping;
    m_pressed.y = m_mapping.y + m_mapping.cy + CFG_OUTER_BORDER;
}

void element_analog_stick::draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data)
{
    element_data_analog_stick* stick = reinterpret_cast<element_data_analog_stick*>(data);
    gs_rect * temp = stick->pressed() ? &m_pressed : &m_mapping;
    vec2 pos = m_pos;
    calc_position(&pos, stick);
    element_texture::draw(effect, image, temp, &pos);
}

void element_analog_stick::calc_position(vec2 * v, element_data_analog_stick* d)
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

void element_data_analog_stick::merge(element_data * other)
{
    if (other && other->get_type() == m_type)
    {
        element_data_analog_stick* other_stick
            = reinterpret_cast<element_data_analog_stick*>(other);

        switch (other_stick->m_data_type)
        {
        case BOTH:
            m_left_stick = other_stick->m_left_stick;
            m_right_stick = other_stick->m_right_stick;
            m_state = other_stick->m_state;
            break;
        case BUTTON_STATE:
            m_state = other_stick->m_state;
            break;
        case STICK_STATE:
            m_left_stick = other_stick->m_left_stick;
            m_right_stick = other_stick->m_right_stick;
            break;
        default: ;
        }
    }
}
