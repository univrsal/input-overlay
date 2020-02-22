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

#include "element_scroll_wheel.hpp"
#include "../util/coordinate_system.hpp"
#include "../util/palette.hpp"
#include "../util/texture.hpp"
#include <utility>

ElementScrollWheel::ElementScrollWheel(const std::string &id, const SDL_Point pos, const SDL_Rect mapping,
									   const uint8_t z)
	: element_texture(ET_WHEEL, id, pos, mapping, z)
{
	refresh_mappings();
}

void ElementScrollWheel::draw(texture *atlas, coordinate_system *cs, const bool selected, const bool alpha)
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

void ElementScrollWheel::handle_event(SDL_Event *event, sdl_helper *helper)
{
	if (event->type == SDL_MOUSEWHEEL) {
		if (event->wheel.y > 0) /* TRIGGER_UP */ {
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

void ElementScrollWheel::update_settings(dialog_new_element *dialog)
{
	element_texture::update_settings(dialog);
	refresh_mappings();
}

void ElementScrollWheel::update_settings(dialog_element_settings *dialog)
{
	element_texture::update_settings(dialog);
	refresh_mappings();
}

ElementScrollWheel *ElementScrollWheel::read_from_file(ccl_config *file, const std::string &id, SDL_Point *default_dim)
{
	return new ElementScrollWheel(id, read_position(file, id), read_mapping(file, id, default_dim),
								  read_layer(file, id));
}
