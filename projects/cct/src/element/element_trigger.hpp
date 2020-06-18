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

#include "element_texture.hpp"

class element_trigger : public element_texture {
public:
	element_trigger()
		: element_texture(),
		  m_side(),
		  m_direction(){
			  /* NO-OP */
		  };

	element_trigger(const std::string &id, SDL_Point pos, SDL_Rect mapping, element_side s, direction d, uint8_t z);

	element_trigger(const std::string &id, SDL_Point pos, SDL_Rect mapping, element_side s, uint8_t z);

	void draw(texture *atlas, coordinate_system *cs, bool selected, bool alpha) override;

	void write_to_json(json_obj &j, SDL_Point *default_dim, uint8_t &layout_flags) override;

	void update_settings(dialog_new_element *dialog) override;

	void update_settings(dialog_element_settings *dialog) override;

	void handle_event(SDL_Event *event, sdl_helper *helper) override;

	static element_trigger *read_from_json(const json &j, SDL_Point *default_dim);

	element_side get_side() const { return m_side; }

	direction get_direction() const { return m_direction; }

	bool get_mode() const { return m_button_mode; }

private:
	void calculate_mappings(SDL_Rect *pressed, SDL_Rect *absolute) const;

	SDL_Rect m_pressed_mapping = {};

	float m_progress = 0;
	element_side m_side;
	direction m_direction;
	bool m_button_mode = false;
};
