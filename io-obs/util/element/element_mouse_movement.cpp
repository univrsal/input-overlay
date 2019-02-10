/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "../../sources/input_source.hpp"
#include "../../../ccl/ccl.hpp"
#include "element_mouse_movement.hpp"
#include "util/layout_constants.hpp"
#include "util/util.hpp"

void element_mouse_movement::load(ccl_config* cfg, const std::string& id)
{
    element_texture::load(cfg, id);
    m_keycode = VC_MOUSE_DATA;
    m_radius = cfg->get_int(id + CFG_MOUSE_RADIUS);
    m_movement_type = cfg->get_int(id + CFG_MOUSE_TYPE) == 0 ? DOT : ARROW;
}

void element_mouse_movement::draw(gs_effect_t* effect,
    gs_image_file_t* image, element_data* data, sources::shared_settings* settings)
{
    if (data)
    {
        const auto element_data = dynamic_cast<element_data_mouse_stats*>(data);

        if (element_data)
        {
            if (m_movement_type == ARROW)
            {
                element_texture::draw(effect, image, &m_mapping, &m_pos, element_data->get_mouse_angle(settings));
            }
            else
            {
                element_data->get_mouse_offset(settings, m_pos, m_offset_pos, m_radius);
                element_texture::draw(effect, image, &m_mapping, &m_offset_pos);
            }
        }
    }
    else
    {
        element_texture::draw(effect, image, &m_mapping, &m_pos);
    }
}

element_data_mouse_stats::element_data_mouse_stats(int16_t x, int16_t y)
    : element_data(MOUSE_MOVEMENT), m_last_x(0), m_last_y(0), m_old_angle(0)
{
    m_x = x;
    m_y = y;
}

bool element_data_mouse_stats::is_persistent()
{
    return true;
}

void element_data_mouse_stats::merge(element_data* other)
{
    if (other && other->get_type() == MOUSE_MOVEMENT)
    {
        const auto data = dynamic_cast<element_data_mouse_stats*>(other);
        if (data)
        {
            m_last_x = m_x;
            m_last_y = m_y;
            m_x = data->m_x;
            m_y = data->m_y;
        }
    }
}

float element_data_mouse_stats::get_mouse_angle(sources::shared_settings* settings)
{
    auto d_x = 0, d_y = 0;

    if (settings->use_center)
    {
        d_x = m_x - settings->monitor_h;
        d_y = m_y - settings->monitor_w;
    }
    else
    {
        d_x = m_x - m_last_x;
        d_y = m_y - m_last_y;
    }
    
    const float new_angle = (0.5 * M_PI) + (atan2f(d_y, d_x));
    if (abs(d_x) < settings->mouse_deadzone || abs(d_y) < settings->mouse_deadzone)
    {
        /* Draw old angle (new movement was to minor) */
        return m_old_angle;
    }

    m_old_angle = new_angle;
    return new_angle;
}

void element_data_mouse_stats::get_mouse_offset(sources::shared_settings* settings,
    const vec2& center, vec2& out, const uint8_t radius) const
{
    auto d_x = 0, d_y = 0;

    if (settings->use_center)
    {
        d_x = m_x - settings->monitor_h;
        d_y = m_y - settings->monitor_w;
    }
    else
    {
        d_x = m_x - m_last_x;
        d_y = m_y - m_last_y;

        if (abs(d_x) < settings->mouse_deadzone)
            d_x = 0;
        if (abs(d_y) < settings->mouse_deadzone)
            d_y = 0;
    }
    

    const auto factor_x = UTIL_CLAMP(-1, ((double)d_x / settings->mouse_sens), 1);
    const auto factor_y = UTIL_CLAMP(-1, ((double)d_y / settings->mouse_sens), 1);

    out.x = center.x + radius * factor_x;
    out.y = center.y + radius * factor_y;

}

element_mouse_movement::element_mouse_movement()
    : element_texture(MOUSE_MOVEMENT), m_movement_type()
{
};
