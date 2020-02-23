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

#include "element.hpp"

class element_texture : public element {
public:
	element_texture(){
		/* NO-OP */
	};

	element_texture(const std::string &id, SDL_Point pos, SDL_Rect mapping, uint8_t z);

	element_error is_valid(notifier *n, sdl_helper *h) override;

	void draw(texture *atlas, coordinate_system *cs, bool selected, bool alpha) override;

	void write_to_json(json &j, SDL_Point *default_dim, uint8_t &layout_flags) override;

	void update_settings(dialog_new_element *dialog) override;

	void update_settings(dialog_element_settings *dialog) override;

	void handle_event(SDL_Event *event, sdl_helper *helper) override{
		/* NO-OP */
	};

	static element_texture *read_from_json(const json &j, SDL_Point *default_dim);

protected:
	element_texture(element_type t, const std::string &id, SDL_Point pos, SDL_Rect mapping, uint8_t z);
};
