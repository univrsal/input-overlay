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

#include "../../util/sdl_helper.hpp"
#include "../dialog.hpp"
#include "gui_element.hpp"
#include <SDL.h>

class dialog;

class button : public gui_element {
public:
	button(int8_t id, int x, int y, const char *text, dialog *parent);

	button(int8_t id, int x, int y, int w, const char *text, dialog *parent);

	~button();

	bool can_select() override;

	void select_state(bool state) override;

	void draw_background() override;

	void draw_foreground() override;

	bool handle_events(SDL_Event *event, bool was_handled) override;

	void close() override;

	void resize() override;

	void refresh() override;

protected:
	bool m_pressed = false;
	bool m_hovered = false;
	bool m_tab_focused = false;

	std::string m_localized_text;
	std::string m_unlocalized_text;
	SDL_Point m_text_pos;
	uint8_t m_font = FONT_WSTRING;
};
