/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "../../sources/input_source.hpp"
#include "element_analog_stick.hpp"
#include "../../../ccl/ccl.hpp"
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
            calc_position(&pos, stick, settings);
            element_texture::draw(effect, image, temp, &pos);
        }
    }
    else
    {
        element_texture::draw(effect, image, nullptr);
    }
}

void element_analog_stick::calc_position(
    vec2* v, element_data_analog_stick* d, sources::shared_settings* settings) const
{
    switch (m_side)
    {
    case SIDE_LEFT:
#if _WIN32
        if (!DEAD_ZONE(d->get_left_stick()->x, settings->left_dz))
#endif
            v->x += d->get_left_stick()->x * m_radius;
#if _WIN32
        if (!DEAD_ZONE(d->get_left_stick()->y, settings->left_dz))
#endif
            v->y += d->get_left_stick()->y * m_radius;
        break;
    case SIDE_RIGHT:
#if _WIN32
        if (!DEAD_ZONE(d->get_right_stick()->x, settings->right_dz))
#endif
            v->x += d->get_right_stick()->x * m_radius;
#if _WIN32
        if (!DEAD_ZONE(d->get_right_stick()->y, settings->right_dz))
#endif
            v->y += d->get_right_stick()->y * m_radius;
        break;
    default: ;
    }
}

void element_data_analog_stick::set_state(const button_state left, const button_state right)
{
    m_left_state = left;
    m_right_state= right;
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
            case SD_BOTH:
                m_left_stick = other_stick->m_left_stick;
                m_right_stick = other_stick->m_right_stick;
                m_left_state = other_stick->m_left_state;
                m_right_state = other_stick->m_right_state;
                break;
            case SD_PRESSED_STATE_LEFT:
                m_left_state = other_stick->m_left_state;
                break;
            case SD_PRESSED_STATE_RIGHT:
                m_right_state = other_stick->m_right_state;
                break;
            case SD_LEFT_X:
                m_left_stick.x = other_stick->m_left_stick.x;
                break;
            case SD_LEFT_Y:
                m_left_stick.y = other_stick->m_left_stick.y;
                break;
            case SD_RIGHT_X:
                m_right_stick.x = other_stick->m_right_stick.x;
                break;
            case SD_RIGHT_Y:
                m_right_stick.y = other_stick->m_right_stick.y;
                break;
            default: ;
            }
        }
    }
}

element_data_analog_stick* element_data_analog_stick::from_buffer(netlib_byte_buf* buffer)
{
    auto result = new element_data_analog_stick();

    if (!netlib_read_float(buffer, &result->m_left_stick.x) ||
        !netlib_read_float(buffer, &result->m_left_stick.y) ||
        !netlib_read_float(buffer, &result->m_right_stick.x) ||
        !netlib_read_float(buffer, &result->m_right_stick.y))
    {
#ifdef _DEBUG
        blog(LOG_INFO, "Reading of analog stick data failed: %s", netlib_get_error());
#endif
        delete result;
        
        return nullptr;
    }
    return result;
}
