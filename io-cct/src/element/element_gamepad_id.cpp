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

#include "element_gamepad_id.hpp"
#include "../dialog/dialog_element_settings.hpp"
#include "../dialog/dialog_new_element.hpp"
#include "../util/coordinate_system.hpp"
#include "../util/palette.hpp"
#include "../util/texture.hpp"

ElementGamepadID::ElementGamepadID(const std::string &id, const SDL_Point pos, const SDL_Rect mapping, const uint8_t z)
    : element_texture(ET_GAMEPAD_ID, id, pos, mapping, z)
{
    /* NO-OP */
}

void ElementGamepadID::draw(texture *atlas, coordinate_system *cs, const bool selected, const bool alpha)
{
	get_abs_dim(cs);
	auto temp = m_mapping;
	temp.x += (temp.w + CFG_INNER_BORDER) * m_last_gamepad_id;
	atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &temp,
	            (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);

	if (m_state) /* Guide button */ {
		temp = m_mapping;
		temp.x += (temp.w + CFG_INNER_BORDER) * 4;
		atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &temp,
		            (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);
	}

	if (selected)
		cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->get_palette()->red());
}

void ElementGamepadID::handle_event(SDL_Event *event, sdl_helper *helper)
{
	switch (event->type) {
	case SDL_CONTROLLERBUTTONDOWN:
	case SDL_CONTROLLERBUTTONUP:
		m_last_gamepad_id = event->caxis.which;
		if (event->cbutton.button == SDL_CONTROLLER_BUTTON_GUIDE)
			m_state = event->type == SDL_CONTROLLERBUTTONDOWN ? BS_PRESSED : BS_RELEASED;
		break;
	case SDL_CONTROLLERAXISMOTION:
		m_last_gamepad_id = event->cbutton.which;
		break;
	default:;
	}
	m_last_gamepad_id = UTIL_CLAMP(0, m_last_gamepad_id, 4);
}

ElementGamepadID *ElementGamepadID::read_from_json(const json &j, SDL_Point *default_dim)
{
	return new ElementGamepadID(j[CFG_ID], read_position(j),
	                            read_mapping(j, default_dim), j[CFG_Z_LEVEL]);
}

void ElementGamepadID::write_to_json(json &j, SDL_Point *default_dim, uint8_t &layout_flags)
{
	element_texture::write_to_json(j, default_dim, layout_flags);
	layout_flags |= OF_GAMEPAD;
}
