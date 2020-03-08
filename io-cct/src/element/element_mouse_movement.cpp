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

#include "element_mouse_movement.hpp"
#include <utility>
#include "../util/notifier.hpp"
#include "../util/sdl_helper.hpp"
#include "../dialog/dialog_new_element.hpp"
#include "../dialog/dialog_element_settings.hpp"
#include "../../../ccl/ccl.hpp"

ElementMouseMovement::ElementMouseMovement(const std::string &id, const SDL_Point pos, const SDL_Rect mapping,
										   const mouse_movement type, const uint16_t radius, const uint8_t z)
	: element_texture(ET_MOUSE_STATS, id, pos, mapping, z)
{
	m_radius = radius;
	m_type = type;
}

element_error ElementMouseMovement::is_valid(notifier *n, sdl_helper *h)
{
	auto error = element_texture::is_valid(n, h);
	if (error == EE_VALID && (m_type == MM_DOT && m_radius == 0)) {
		n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_RADIUS_INVALID));
		error = EE_MOUSE_RADIUS;
	}
	return error;
}

void ElementMouseMovement::write_to_file(ccl_config *cfg, SDL_Point *default_dim, uint8_t &layout_flags)
{
	element_texture::write_to_file(cfg, default_dim, layout_flags);
	auto comment = "Movement type of " + m_id;

	cfg->add_int(m_id + CFG_MOUSE_TYPE, comment, m_type, true);
	if (m_type == MM_DOT) {
		comment = "Movement radius of " + m_id;
		cfg->add_int(m_id + CFG_MOUSE_RADIUS, comment, m_radius, true);
	}

	layout_flags |= OF_MOUSE;
}

void ElementMouseMovement::update_settings(dialog_new_element *dialog)
{
	element_texture::update_settings(dialog);
	m_radius = dialog->get_radius();
	m_type = dialog->get_mouse_type();
}

mouse_movement ElementMouseMovement::get_mouse_type() const
{
	return m_type;
}

uint16_t ElementMouseMovement::get_radius() const
{
	return m_radius;
}

ElementMouseMovement *ElementMouseMovement::read_from_file(ccl_config *file, const std::string &id,
														   SDL_Point *default_dim)
{
	auto mmt = MM_DOT;
	if (file->get_int(id + CFG_MOUSE_TYPE) != 0)
		mmt = MM_ARROW;

	return new ElementMouseMovement(id, read_position(file, id), read_mapping(file, id, default_dim), mmt,
									file->get_int(id + CFG_MOUSE_RADIUS), read_layer(file, id));
}
