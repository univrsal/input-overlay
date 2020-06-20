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

#define ITEM_V_SPACE 4

class combobox : public gui_element {
public:
	combobox(int8_t id, int x, int y, int w, int h, dialog *parent, uint16_t flags = 0x0);

	void close() override;

	void draw_background() override;

	void draw_foreground() override;

	void add_item(std::string item)
	{
		if (m_flags & ELEMENT_UNLOCALIZED)
			m_items.emplace_back(item);
		else
			m_items.emplace_back(get_helper()->loc(item.c_str()));

		m_item_box = {get_left(), get_bottom() - 1, get_width(),
					  static_cast<int>(m_items.size() * m_item_v_space + ITEM_V_SPACE)};
	};

	uint8_t get_selected() const { return m_selected_id; }

	void select_item(const uint8_t id)
	{
		m_selected_id = UTIL_CLAMP(0, id, m_items.size() - 1);
		m_parent_dialog->action_performed(ACTION_COMBO_ITEM_SELECTED);
	}

	void toggle_list();

	bool can_select() override;

	void select_state(bool state) override;

	bool handle_events(SDL_Event *event, bool was_handled) override;

	bool is_mouse_over_list(const SDL_Point *p = nullptr) const;

	bool is_mouse_over(const SDL_Point *p = nullptr) override;

	void cycle_up(bool select);

	void cycle_down(bool select);

	void resize() override;

private:
	std::vector<std::string> m_items;
	SDL_Rect m_item_box;
	uint8_t m_font = FONT_WSTRING;
	uint8_t m_selected_id = 0;
	uint8_t m_hovered_id = 0;
	uint8_t m_item_v_space = 0;

	bool m_focused = false;
	bool m_list_open = false;
};
