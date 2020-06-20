/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
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

#include "../dialog.hpp"
#include "gui_element.hpp"

#define CHECKBOX_SIZE 17

class checkbox : public gui_element {
public:
	checkbox(int8_t id, int x, int y, const char *text, dialog *parent);

	checkbox(int8_t id, int x, int y, const char *text, dialog *parent, bool state);

	~checkbox();

	void close() override;

	bool can_select() override;

	void select_state(bool state) override;

	void draw_background() override;

	void draw_foreground() override;

	bool handle_events(SDL_Event *event, bool was_handled) override;

	bool get_state() const { return m_state; }

	void set_checked(bool state);

private:
	bool m_focused = false;
	bool m_state = false;
	label *m_label = nullptr;
	SDL_Rect m_checkbox;
};
