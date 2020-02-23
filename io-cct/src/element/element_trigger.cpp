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

#include "element_trigger.hpp"
#include "../dialog/dialog_element_settings.hpp"
#include "../dialog/dialog_new_element.hpp"
#include "../util/coordinate_system.hpp"
#include "../util/palette.hpp"
#include "../util/texture.hpp"
#include <utility>

element_trigger::element_trigger(const std::string &id, const SDL_Point pos, const SDL_Rect mapping,
                                 const element_side s, const direction d, const uint8_t z)
    : element_texture(ET_TRIGGER, id, pos, mapping, z)
{
	m_pressed_mapping = m_mapping;
	m_pressed_mapping.y += m_mapping.h + CFG_INNER_BORDER;
	m_direction = d;
	m_side = s;
}

element_trigger::element_trigger(const std::string &id, const SDL_Point pos, const SDL_Rect mapping,
                                 const element_side s, const uint8_t z)
    : element_texture(ET_TRIGGER, id, pos, mapping, z), m_direction()
{
	m_pressed_mapping = m_mapping;
	m_pressed_mapping.y += m_mapping.h + CFG_INNER_BORDER;
	m_side = s;
	m_button_mode = true;
}

void element_trigger::draw(texture *atlas, coordinate_system *cs, const bool selected, const bool alpha)
{
	get_abs_dim(cs);

	if (m_button_mode && m_progress >= .2f) {
		atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_pressed_mapping,
		            alpha ? ELEMENT_HIDE_ALPHA : 255);
	} else if (!m_button_mode && m_progress > 0.f) {
		atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping,
		            (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);
		auto temp = m_pressed_mapping;
		auto temp2 = m_dimensions_scaled;
		calculate_mappings(&temp, &temp2);
		atlas->draw(cs->get_helper()->renderer(), &temp2, &temp, (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);
	} else {
		atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping,
		            (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);
	}

	if (selected)
		cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->get_palette()->red());
}

void element_trigger::write_to_json(json &j, SDL_Point *default_dim, uint8_t &layout_flags)
{
	element_texture::write_to_json(j, default_dim, layout_flags);
	j[CFG_TRIGGER_MODE] = m_button_mode;
	j[CFG_SIDE] = static_cast<int>(m_side);

	if (!m_button_mode)
		j[CFG_DIRECTION] = static_cast<int>(m_direction);
}

void element_trigger::update_settings(dialog_new_element *dialog)
{
	element_texture::update_settings(dialog);
	m_pressed_mapping = m_mapping;
	m_pressed_mapping.y += m_mapping.h + CFG_INNER_BORDER;
	m_button_mode = dialog->get_trigger_mode();
	m_side = dialog->get_side();
	m_direction = dialog->get_direction();
}

void element_trigger::update_settings(dialog_element_settings *dialog)
{
	element_texture::update_settings(dialog);
	m_pressed_mapping = m_mapping;
	m_pressed_mapping.y += m_mapping.h + CFG_INNER_BORDER;
}

void element_trigger::handle_event(SDL_Event *event, sdl_helper *helper)
{
	if (event->type == SDL_CONTROLLERAXISMOTION) {
		if (m_side == ES_LEFT && event->caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT ||
		    m_side == ES_RIGHT && event->caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
			m_progress = static_cast<float>(event->caxis.value) / AXIS_MAX_AMPLITUDE;
		}
	}
}

element_trigger *element_trigger::read_from_json(const json &j, SDL_Point *default_dim)
{
	const auto button_mode = j[CFG_TRIGGER_MODE];
	const auto s = read_side(j);

	if (button_mode) {
		return new element_trigger(j[CFG_ID], read_position(j), read_mapping(j, default_dim), s,
		                           j[CFG_Z_LEVEL]);
	}
	auto d = static_cast<direction>(UTIL_CLAMP(0, (int)j[CFG_DIRECTION], DIR_MAX - 1));
	return new element_trigger(j[CFG_ID], read_position(j), read_mapping(j, default_dim), s, d,
	                           j[CFG_Z_LEVEL]);
}

void element_trigger::calculate_mappings(SDL_Rect *pressed, SDL_Rect *absolute) const
{
	switch (m_direction) {
	default:
	case DIR_UP:
		pressed->h = m_mapping.h * m_progress;
		pressed->y = m_pressed_mapping.y + (m_mapping.h - pressed->h);
		absolute->y += (m_mapping.h - pressed->h) * m_scale;
		absolute->h = pressed->h * m_scale;
		break;
	case DIR_DOWN:
		pressed->h = m_mapping.h * m_progress;
		absolute->h = pressed->h * m_scale;
		break;
	case DIR_LEFT:
		pressed->w = m_mapping.w * m_progress;
		pressed->x = m_mapping.x + (m_mapping.w - pressed->w);
		absolute->x += (m_mapping.w - pressed->w) * m_scale;
		absolute->w = pressed->w * m_scale;
		break;
	case DIR_RIGHT:
		pressed->w = m_mapping.w * m_progress;
		absolute->w = pressed->w * m_scale;
		break;
	}
}
