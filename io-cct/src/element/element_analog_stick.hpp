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

#pragma once

#define STICK_RESET 500

#include "../util/util.hpp"
#include "element_texture.hpp"

class ElementAnalogStick : public element_texture {
public:
	ElementAnalogStick() : element_texture(), m_static_scaled(), m_stick() { /* NO-OP */ }

	ElementAnalogStick(const std::string &id, SDL_Point pos, SDL_Rect mapping, element_side side, uint8_t radius,
	                   uint8_t z);

	SDL_Rect *get_abs_dim(coordinate_system *cs) override;

	element_error is_valid(notifier *n, sdl_helper *h) override;

	void draw(texture *atlas, coordinate_system *cs, bool selected, bool alpha) override;

	void write_to_json(json &j, SDL_Point *default_dim, uint8_t &flags) override;

	void update_settings(dialog_new_element *dialog) override;

	void update_settings(dialog_element_settings *dialog) override;

	void handle_event(SDL_Event *event, sdl_helper *helper) override;

	element_side get_stick() const { return m_stick; }

	uint16_t get_radius() const { return m_radius; }

	static ElementAnalogStick *read_from_json(const json &j, SDL_Point *default_dim);

private:
	SDL_Rect m_static_scaled; /* Position without input movement for display */
	element_side m_stick;
	bool m_pressed = false;
	float m_x_axis = 0;
	float m_y_axis = 0;
	uint16_t m_radius = 0;

	Timer m_movement_reset;
};
