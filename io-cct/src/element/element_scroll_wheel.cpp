/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "element_scroll_wheel.hpp"
#include <utility>
#include "../util/texture.hpp"
#include "../util/coordinate_system.hpp"
#include "../util/palette.hpp"

ElementScrollWheel::ElementScrollWheel(const std::string &id, const SDL_Point pos, const SDL_Rect mapping,
                                       const uint8_t z) : element_texture(MOUSE_SCROLLWHEEL, id, pos, mapping, z)
{
    refresh_mappings();
}

void ElementScrollWheel::draw(texture* atlas, coordinate_system* cs, const bool selected, const bool alpha)
{
    get_abs_dim(cs);

    if (m_pressed)
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping_pressed,
                    (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);

    if (m_down)
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping_down,
                    (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);

    if (m_up)
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping_up,
                    (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);

    atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping,
                (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);

    if (selected)
        cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->get_palette()->red());

    if (m_wheel_reset.started() && m_wheel_reset.get_time() >= WHEEL_RESET) {
        m_wheel_reset.stop();
        m_up = false;
        m_down = false;
    }
}

void ElementScrollWheel::handle_event(SDL_Event* event, sdl_helper* helper)
{
    if (event->type == SDL_MOUSEWHEEL) {
        if (event->wheel.y > 0) /* TRIGGER_UP */
        {
            m_up = true;
            m_down = false;
        } else {
            m_down = true;
            m_up = false;
        }
        m_wheel_reset.start();
    } else if (event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP) {
        if (event->button.button == SDL_BUTTON_MIDDLE)
            m_pressed = event->type == SDL_MOUSEBUTTONDOWN;
    }
}

void ElementScrollWheel::refresh_mappings()
{
    m_mapping_up = m_mapping;
    m_mapping_down = m_mapping;
    m_mapping_pressed = m_mapping;

    m_mapping_up.x += (m_mapping.w + CFG_INNER_BORDER) * POS_WHEEL_UP;
    m_mapping_down.x += (m_mapping.w + CFG_INNER_BORDER) * POS_WHEEL_DOWN;
    m_mapping_pressed.x += (m_mapping.w + CFG_INNER_BORDER) * POS_WHEEL_PRESSED;
}

void ElementScrollWheel::update_settings(dialog_new_element* dialog)
{
    element_texture::update_settings(dialog);
    refresh_mappings();
}

void ElementScrollWheel::update_settings(dialog_element_settings* dialog)
{
    element_texture::update_settings(dialog);
    refresh_mappings();
}

ElementScrollWheel* ElementScrollWheel::read_from_file(ccl_config* file, const std::string &id, SDL_Point* default_dim)
{
    return new ElementScrollWheel(id, read_position(file, id), read_mapping(file, id, default_dim),
                                  read_layer(file, id));
}
