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

class ElementDPad : public element_texture {
public:
	ElementDPad(){
		/* NO-OP */
	};

	ElementDPad(const std::string &id, const SDL_Point pos, const SDL_Rect mapping, const uint8_t z)
		: element_texture(id, pos, mapping, z)
	{
		/* NO-OP */
	}

	void draw(texture *atlas, coordinate_system *cs, bool selected, bool alpha) override;

	void handle_event(SDL_Event *event, sdl_helper *helper) override;

	static ElementDPad *read_from_file(ccl_config *file, const std::string &id, SDL_Point *default_dim);

	void write_to_file(ccl_config *cfg, SDL_Point *default_dim, uint8_t &layout_flags) override;

private:
	uint8_t m_dir = DD_CENTER;
	int8_t m_last_button = SDL_CONTROLLER_BUTTON_INVALID;
};
