/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <universailp@web.de>.
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

void ElementDPad::draw(texture *atlas, coordinate_system *cs, const bool selected, const bool alpha)
{
	get_abs_dim(cs);
	if (m_dir == DD_CENTER) {
		atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping,
					(alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);
	} else {
		auto temp = m_mapping;
		temp.x += (CFG_INNER_BORDER + temp.w) * m_dir;
		atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &temp,
					(alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);
	}

	if (selected)
		cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->get_palette()->red());
}

void ElementDPad::handle_event(SDL_Event *event, sdl_helper *helper)
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

ElementDPad *ElementDPad::read_from_json(const json &j, SDL_Point *default_dim)
{
	return new ElementDPad(j[CFG_ID], read_position(j), read_mapping(j, default_dim), j[CFG_Z_LEVEL]);
}

void ElementDPad::write_to_json(json &j, SDL_Point *default_dim, uint8_t &layout_flags)
{
	element_texture::write_to_json(j, default_dim, layout_flags);
	layout_flags |= OF_GAMEPAD;
}
