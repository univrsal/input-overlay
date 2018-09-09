/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "ElementScrollWheel.hpp"
#include <utility>
#include "../util/Texture.hpp"
#include "../util/CoordinateSystem.hpp"

ElementScrollWheel::ElementScrollWheel(std::string id, SDL_Point pos, SDL_Rect mapping, uint8_t z)
    : ElementTexture(ElementType::MOUSE_SCROLLWHEEL, std::move(id), pos, mapping, z)
{
    refresh_mappings();
}

void ElementScrollWheel::draw(Texture * atlas, CoordinateSystem * cs, bool selected, bool alpha)
{
    get_abs_dim(cs);

    if (m_pressed)
        atlas->draw(cs->get_helper()->renderer(),
            &m_dimensions_scaled, &m_mapping_pressed, alpha ? ELEMENT_HIDE_ALPHA : 255);

    if (m_down)
        atlas->draw(cs->get_helper()->renderer(),
            &m_dimensions_scaled, &m_mapping_down, alpha ? ELEMENT_HIDE_ALPHA : 255);

    if (m_up)
        atlas->draw(cs->get_helper()->renderer(),
            &m_dimensions_scaled, &m_mapping_up, alpha ? ELEMENT_HIDE_ALPHA : 255);

    atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping, alpha ? ELEMENT_HIDE_ALPHA : 255);

    if (selected)
        cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->palette()->red());

    if (m_wheel_reset.started() && m_wheel_reset.get_time() >= WHEEL_RESET)
    {
        m_wheel_reset.stop();
        m_up = false;
        m_down = false;
    }
}

void ElementScrollWheel::handle_event(SDL_Event * event, SDL_Helper * helper)
{
    if (event->type == SDL_MOUSEWHEEL)
    {
        if (event->wheel.y > 0) /* UP */
        {
            m_up = true;
            m_down = false;
        }
        else
        {
            m_down = true;
            m_up = false;
        }
        m_wheel_reset.start();
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN
        || event->type == SDL_MOUSEBUTTONUP)
    {
        if (event->button.button == SDL_BUTTON_MIDDLE)
            m_pressed = event->type == SDL_MOUSEBUTTONDOWN;
    }
}

void ElementScrollWheel::refresh_mappings(void)
{
    m_mapping_up = m_mapping;
    m_mapping_down = m_mapping;
    m_mapping_pressed = m_mapping;

    m_mapping_up.x += (m_mapping.w + CFG_INNER_BORDER) * POS_WHEEL_UP;
    m_mapping_down.x += (m_mapping.w + CFG_INNER_BORDER) * POS_WHEEL_DOWN;
    m_mapping_pressed.x += (m_mapping.w + CFG_INNER_BORDER) * POS_WHEEL_PRESSED;
}

void ElementScrollWheel::update_settings(DialogNewElement * dialog)
{
    ElementTexture::update_settings(dialog);
    refresh_mappings();
}

void ElementScrollWheel::update_settings(DialogElementSettings * dialog)
{
    ElementTexture::update_settings(dialog);
    refresh_mappings();
}

ElementScrollWheel * ElementScrollWheel::read_from_file(ccl_config * file, const std::string& id, SDL_Point * default_dim)
{
    return new ElementScrollWheel(id, Element::read_position(file, id),
        Element::read_mapping(file, id, default_dim), Element::read_layer(file, id));
}
