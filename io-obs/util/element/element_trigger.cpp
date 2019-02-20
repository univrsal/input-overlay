/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "../../sources/input_source.hpp"
#include "../../../ccl/ccl.hpp"
#include "element_trigger.hpp"
#include "../util.hpp"
#include "util/layout_constants.hpp"

element_trigger::element_trigger()
    : element_texture(TRIGGER)
{
};

void element_trigger::load(ccl_config* cfg, const std::string& id)
{
    element_texture::load(cfg, id);
    m_button_mode = cfg->get_bool(id + CFG_TRIGGER_MODE);
    m_side = static_cast<element_side>(cfg->get_int(id + CFG_SIDE));
    m_keycode = VC_TRIGGER_DATA;
    m_pressed = m_mapping;
    m_pressed.y = m_mapping.y + m_mapping.cy + CFG_INNER_BORDER;
    if (!m_button_mode)
    {
        m_direction = static_cast<trigger_direction>(cfg->get_int(
            id + CFG_DIRECTION));
    }
}

void element_trigger::draw(gs_effect_t* effect,
    gs_image_file_t* image, element_data* data, sources::overlay_settings* settings)
{
    if (data)
    {
        const auto trigger = dynamic_cast<element_data_trigger*>(data);
        auto progress = 0.f;
        if (trigger)
        {
            switch (m_side)
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
                if (progress >= 0.1)
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

data_source element_trigger::get_source()
{
    return GAMEPAD;
}

void element_trigger::calculate_mapping(gs_rect* pressed, vec2* pos, const float progress) const
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

element_data_trigger::element_data_trigger(const trigger_data_type side, const float val)
    : element_data(TRIGGER)
{
    if (side == T_DATA_LEFT)
        m_left_trigger = val;
    else
        m_right_trigger = val;
    m_data_type = side;
}

element_data_trigger::element_data_trigger(const float left, const float right)
    : element_data(TRIGGER)
{
    m_left_trigger = left;
    m_right_trigger = right;
    m_data_type = T_DATA_BOTH;
}

float element_data_trigger::get_left() const
{
    return m_left_trigger;
}

float element_data_trigger::get_right() const
{
    return m_right_trigger;
}

bool element_data_trigger::is_persistent()
{
    return true;
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

element_data_trigger* element_data_trigger::from_buffer(netlib_byte_buf* buffer)
{
    uint8_t left, right;
    if (!netlib_read_uint8(buffer, &left) || !netlib_read_uint8(buffer, &right))
    {
#ifdef _DEBUG
        blog(LOG_INFO, "Failed to read trigger data: %s", netlib_get_error());
#endif
        return nullptr;
    }
    return new element_data_trigger(left / TRIGGER_MAX_VAL, right / TRIGGER_MAX_VAL);
}
