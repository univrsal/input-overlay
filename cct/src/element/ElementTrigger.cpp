/**
 * Created by universal on 06.09.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "ElementTrigger.hpp"
#include <utility>
#include "../dialog/DialogNewElement.hpp"
#include "../dialog/DialogElementSettings.hpp"
#include "../util/CoordinateSystem.hpp"
#include "../util/Texture.hpp"
#include "../../../ccl/ccl.hpp"

ElementTrigger::ElementTrigger(const std::string& id, const SDL_Point pos, const SDL_Rect mapping,
                               const element_side s, const trigger_direction d, const uint8_t z)
    : ElementTexture(TRIGGER, id, pos, mapping, z)
{
    m_pressed_mapping = m_mapping;
    m_pressed_mapping.y += m_mapping.h + CFG_INNER_BORDER;
    m_direction = d;
    m_side = s;
}

ElementTrigger::ElementTrigger(const std::string& id, const SDL_Point pos, const SDL_Rect mapping,
                               const element_side s, const uint8_t z)
    : ElementTexture(TRIGGER, id, pos, mapping, z), m_direction()
{
    m_pressed_mapping = m_mapping;
    m_pressed_mapping.y += m_mapping.h + CFG_INNER_BORDER;
    m_side = s;
    m_button_mode = true;
}

void ElementTrigger::draw(Texture* atlas, CoordinateSystem* cs, const bool selected, const bool alpha)
{
    get_abs_dim(cs);

    if (m_button_mode && m_progress >= .2f)
    {
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled,
                    &m_pressed_mapping, alpha ? ELEMENT_HIDE_ALPHA : 255);
    }
    else if (!m_button_mode && m_progress > 0.f)
    {
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled,
                    &m_mapping, alpha ? ELEMENT_HIDE_ALPHA : 255);
        auto temp = m_pressed_mapping;
        auto temp2 = m_dimensions_scaled;
        calculate_mappings(&temp, &temp2);
        atlas->draw(cs->get_helper()->renderer(), &temp2,
                    &temp, alpha ? ELEMENT_HIDE_ALPHA : 255);
    }
    else
    {
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled,
                    &m_mapping, alpha ? ELEMENT_HIDE_ALPHA : 255);
    }

    
    if (selected)
        cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->palette()->red());
}

void ElementTrigger::write_to_file(ccl_config* cfg, SDL_Point* default_dim)
{
    ElementTexture::write_to_file(cfg, default_dim);
    auto comment = "Trigger mode of " + m_id;
    cfg->add_bool(m_id + CFG_TRIGGER_MODE, comment, m_button_mode, true);
    comment = "Trigger side of " + m_id;
    cfg->add_int(m_id + CFG_SIDE, comment, static_cast<int>(m_side), true);
    
    if (!m_button_mode)
    {
        comment = "Trigger side of " + m_id;
        cfg->add_int(m_id + CFG_DIRECTION, comment, static_cast<int>(m_direction), true);
    }
}

void ElementTrigger::update_settings(DialogNewElement* dialog)
{
    ElementTexture::update_settings(dialog);
    m_pressed_mapping = m_mapping;
    m_pressed_mapping.y += m_mapping.h + CFG_INNER_BORDER;
    m_button_mode = dialog->get_trigger_mode();
    m_side = dialog->get_side();
    m_direction = dialog->get_direction();
}

void ElementTrigger::update_settings(DialogElementSettings* dialog)
{
    ElementTexture::update_settings(dialog);
    m_pressed_mapping = m_mapping;
    m_pressed_mapping.y += m_mapping.h + CFG_INNER_BORDER;
}

void ElementTrigger::handle_event(SDL_Event* event, SDL_Helper* helper)
{
    if (event->type == SDL_CONTROLLERAXISMOTION)
    {
        if (m_side == SIDE_LEFT && event->caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT
            || m_side == SIDE_RIGHT && event->caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
        {
            m_progress = static_cast<float>(event->caxis.value) / AXIS_MAX_AMPLITUDE;
        }
    }
}

ElementTrigger* ElementTrigger::read_from_file(ccl_config* file, const std::string& id, SDL_Point* default_dim)
{
    const auto button_mode = file->get_bool(id + CFG_TRIGGER_MODE);
    const auto s = read_side(file, id);

    if (button_mode)
    {
        return new ElementTrigger(id, read_position(file, id),
                                  read_mapping(file, id, default_dim), s, read_layer(file, id));
    }
    trigger_direction d;
    switch (file->get_int(id + CFG_DIRECTION))
    {
    case TRIGGER_UP:
        d = TRIGGER_UP;
        break;
    case TRIGGER_DOWN:
        d = TRIGGER_DOWN;
        break;
    case TRIGGER_RIGHT:
        d = TRIGGER_RIGHT;
        break;
    default:
    case TRIGGER_LEFT:
        d = TRIGGER_LEFT;
    }
    return new ElementTrigger(id, read_position(file, id),
                              read_mapping(file, id, default_dim), s, d, read_layer(file, id));
}

void ElementTrigger::calculate_mappings(SDL_Rect* pressed, SDL_Rect* absolute) const
{
    switch (m_direction)
    {
    case TRIGGER_UP:
        pressed->h = m_mapping.h * m_progress;
        pressed->y = m_pressed_mapping.y + (m_mapping.h - pressed->h);
        absolute->y += (m_mapping.h - pressed->h) * m_scale;
        absolute->h = pressed->h * m_scale;
        break;
    case TRIGGER_DOWN:
        pressed->h = m_mapping.h * m_progress;
        absolute->h = pressed->h * m_scale;
        break;
    case TRIGGER_LEFT:
        pressed->w = m_mapping.w * m_progress;
        pressed->x = m_mapping.x + (m_mapping.w - pressed->w);
        absolute->x += (m_mapping.w - pressed->w) * m_scale;
        absolute->w = pressed->w * m_scale;
        break;
    case TRIGGER_RIGHT:
        pressed->w = m_mapping.w * m_progress;
        absolute->w = pressed->w * m_scale;
        break;
    }
}
