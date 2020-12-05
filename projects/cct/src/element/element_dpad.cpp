/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#include "element_dpad.hpp"
#include "../dialog/dialog_new_element.hpp"
#include "../util/coordinate_system.hpp"
#include "../util/palette.hpp"
#include "../util/texture.hpp"

inline dpad_texture from_direction(uint8_t dir)
{
    if (dir & DD_UP) {
        if (dir & DD_LEFT)
            return DT_TOP_LEFT;
        else if (dir & DD_RIGHT)
            return DT_TOP_RIGHT;
        return DT_UP;
    } else if (dir & DD_DOWN) {
        if (dir & DD_LEFT)
            return DT_BOTTOM_LEFT;
        else if (dir & DD_RIGHT)
            return DT_BOTTOM_RIGHT;
        return DT_DOWN;
    } else if (dir & DD_LEFT) {
        return DT_LEFT;
    } else if (dir & DD_RIGHT) {
        return DT_RIGHT;
    }
    return DT_CENTER;
}

void element_dpad::draw(texture *atlas, coordinate_system *cs, const bool selected, const bool alpha)
{
    get_abs_dim(cs);
    if (m_dir == DD_CENTER) {
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping,
                    (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);
    } else {
        auto temp = m_mapping;
        temp.x += (CFG_INNER_BORDER + temp.w) * from_direction(m_dir);
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &temp,
                    (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);
    }

    if (selected)
        cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->get_palette()->red());
}

void element_dpad::handle_event(SDL_Event *event, sdl_helper *helper)
{
    if (event->type == SDL_CONTROLLERBUTTONDOWN) {
        switch (event->cbutton.button) {
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            m_dir |= DD_UP;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            m_dir |= DD_DOWN;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            m_dir |= DD_LEFT;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            m_dir |= DD_RIGHT;
            break;
        default:;
        }
    } else if (event->type == SDL_CONTROLLERBUTTONUP) {
        switch (event->cbutton.button) {
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            m_dir &= ~DD_UP;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            m_dir &= ~DD_DOWN;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            m_dir &= ~DD_LEFT;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            m_dir &= ~DD_RIGHT;
            break;
        default:;
        }
    }
}

element_dpad *element_dpad::read_from_json(const json &j, SDL_Point *default_dim)
{
    return new element_dpad(j[CFG_ID].string_value(), read_position(j), read_mapping(j, default_dim),
                            uint8_t(j[CFG_Z_LEVEL].number_value()));
}

void element_dpad::write_to_json(json_obj &j, SDL_Point *default_dim, uint8_t &layout_flags)
{
    element_texture::write_to_json(j, default_dim, layout_flags);
    layout_flags |= OF_GAMEPAD;
}
