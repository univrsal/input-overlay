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

#include "element_analog_stick.hpp"
#include "../dialog/dialog_element_settings.hpp"
#include "../dialog/dialog_new_element.hpp"
#include "../util/coordinate_system.hpp"
#include "../util/notifier.hpp"
#include "../util/palette.hpp"
#include "../util/sdl_helper.hpp"
#include "../util/texture.hpp"

ElementAnalogStick::ElementAnalogStick(const std::string &id, const SDL_Point pos, const SDL_Rect mapping,
									   const element_side side, const uint8_t radius, const uint8_t z)
	: element_texture(ET_ANALOG_STICK, id, pos, mapping, z), m_static_scaled()
{
	m_stick = side;
	m_radius = radius;
}

SDL_Rect *ElementAnalogStick::get_abs_dim(coordinate_system *cs)
{
	m_static_scaled = *element::get_abs_dim(cs);

	m_dimensions_scaled.x += m_x_axis * m_radius * cs->get_scale();
	m_dimensions_scaled.y += m_y_axis * m_radius * cs->get_scale();
	return &m_dimensions_scaled;
}

element_error ElementAnalogStick::is_valid(notifier *n, sdl_helper *h)
{
	auto error = element_texture::is_valid(n, h);

	if (error == EE_VALID && m_radius == 0) {
		n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_RADIUS_INVALID));
		error = EE_STICK_RADIUS;
	}
	return error;
}

void ElementAnalogStick::draw(texture *atlas, coordinate_system *cs, const bool selected, const bool alpha)
{
	get_abs_dim(cs);
	if (m_pressed) {
		auto temp = m_mapping;
		temp.y += temp.h + CFG_INNER_BORDER;
		atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &temp,
					(alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);
	} else {
		atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping,
					(alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);
	}

	if (selected) {
		cs->get_helper()->util_draw_rect(&m_static_scaled, cs->get_helper()->get_palette()->red());
	}

	if (m_movement_reset.started() && m_movement_reset.get_time() >= STICK_RESET) {
		m_movement_reset.stop();
		m_x_axis = 0.f, m_y_axis = 0.f;
	}
}

void ElementAnalogStick::write_to_json(json &j, SDL_Point *default_dim, uint8_t &flags)
{
	element_texture::write_to_json(j, default_dim, flags);
	j[CFG_SIDE] = static_cast<int>(m_stick);
	j[CFG_STICK_RADIUS] = static_cast<int>(m_radius);
	flags |= OF_GAMEPAD | (m_stick == ES_LEFT ? OF_LEFT_STICK : OF_RIGHT_STICK);
}

void ElementAnalogStick::update_settings(dialog_new_element *dialog)
{
	element_texture::update_settings(dialog);
	m_radius = dialog->get_radius();
	m_stick = dialog->get_side();
}

void ElementAnalogStick::update_settings(dialog_element_settings *dialog)
{
	element_texture::update_settings(dialog);
}

void ElementAnalogStick::handle_event(SDL_Event *event, sdl_helper *helper)
{
	if (event->type == SDL_CONTROLLERAXISMOTION && SDL_abs(event->caxis.value) >= STICK_DEAD_ZONE) {
		if (m_stick == ES_LEFT) {
			if (event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
				m_x_axis = ((float)event->caxis.value) / AXIS_MAX_AMPLITUDE;
				m_movement_reset.start();
			} else if (event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
				m_y_axis = static_cast<float>(event->caxis.value) / AXIS_MAX_AMPLITUDE;
				m_movement_reset.start();
			}
		} else {
			if (event->caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX) {
				m_x_axis = static_cast<float>(event->caxis.value) / AXIS_MAX_AMPLITUDE;
				m_movement_reset.start();
			} else if (event->caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY) {
				m_y_axis = static_cast<float>(event->caxis.value) / AXIS_MAX_AMPLITUDE;
				m_movement_reset.start();
			}
		}
	} else if (event->type == SDL_CONTROLLERBUTTONDOWN || event->type == SDL_CONTROLLERBUTTONUP) {
		if (m_stick == ES_LEFT) {
			if (event->cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSTICK)
				m_pressed = event->cbutton.state == SDL_PRESSED;
		} else {
			if (event->cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSTICK)
				m_pressed = event->cbutton.state == SDL_PRESSED;
		}
	}
}

ElementAnalogStick *ElementAnalogStick::read_from_json(const json &j, SDL_Point *default_dim)
{
	const auto s = read_side(j);

	return new ElementAnalogStick(j[CFG_ID], read_position(j), read_mapping(j, default_dim), s, j[CFG_STICK_RADIUS],
								  j[CFG_Z_LEVEL]);
}
