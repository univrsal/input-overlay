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

class ElementButton : public element_texture {
public:
	ElementButton() : element_texture(), m_pressed_mapping() { /* NO-OP */ }

	ElementButton(const std::string &id, SDL_Point pos, SDL_Rect mapping, uint16_t vc, uint8_t z);

	element_error is_valid(notifier *n, sdl_helper *h) override;

	void draw(texture *atlas, coordinate_system *cs, bool selected, bool alpha) override;

	void write_to_file(ccl_config *cfg, SDL_Point *default_dim, uint8_t &layout_flags) override;

	void update_settings(dialog_new_element *dialog) override;

	void update_settings(dialog_element_settings *dialog) override;

	void handle_event(SDL_Event *event, sdl_helper *helper) override;

	int get_vc() override { return m_keycode; }

	static ElementButton *read_from_file(ccl_config *file, const std::string &id, SDL_Point *default_dim);

private:
	SDL_Rect m_pressed_mapping;
	uint16_t m_keycode = 0;
	bool m_pressed = false;
};
