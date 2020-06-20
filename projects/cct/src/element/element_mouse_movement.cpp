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

#include "element_mouse_movement.hpp"
#include "../dialog/dialog_element_settings.hpp"
#include "../dialog/dialog_new_element.hpp"
#include "../util/notifier.hpp"
#include "../util/sdl_helper.hpp"
#include <utility>

element_mouse_movement::element_mouse_movement(const std::string &id, const SDL_Point pos, const SDL_Rect mapping,
											   const mouse_movement type, const uint16_t radius, const uint8_t z)
	: element_texture(ET_MOUSE_STATS, id, pos, mapping, z)
{
	m_radius = radius;
	m_type = type;
}

element_error element_mouse_movement::is_valid(notifier *n, sdl_helper *h)
{
	auto error = element_texture::is_valid(n, h);
	if (error == EE_VALID && (m_type == MM_DOT && m_radius == 0)) {
		n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_RADIUS_INVALID));
		error = EE_MOUSE_RADIUS;
	}
	return error;
}

void element_mouse_movement::write_to_json(json_obj &j, SDL_Point *default_dim, uint8_t &layout_flags)
{
	element_texture::write_to_json(j, default_dim, layout_flags);

	j[CFG_MOUSE_TYPE] = m_type;
	if (m_type == MM_DOT) {
		j[CFG_MOUSE_RADIUS] = m_radius;
	}
	layout_flags |= OF_MOUSE;
}

void element_mouse_movement::update_settings(dialog_new_element *dialog)
{
	element_texture::update_settings(dialog);
	m_radius = dialog->get_radius();
	m_type = dialog->get_mouse_type();
}

mouse_movement element_mouse_movement::get_mouse_type() const
{
	return m_type;
}

uint16_t element_mouse_movement::get_radius() const
{
	return m_radius;
}

element_mouse_movement *element_mouse_movement::read_from_json(const json &j, SDL_Point *default_dim)
{
	auto mmt = MM_DOT;
	if (j[CFG_MOUSE_TYPE] != 0)
		mmt = MM_ARROW;

	return new element_mouse_movement(j[CFG_ID].string_value(), read_position(j), read_mapping(j, default_dim), mmt,
									  j[CFG_MOUSE_RADIUS].number_value(), j[CFG_Z_LEVEL].number_value());
}
