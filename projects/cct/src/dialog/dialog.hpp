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

#include "../util/sdl_helper.hpp"
#include "elements/gui_element.hpp"
#include "elements/label.hpp"
#include <memory>
#include <string>
#include <vector>

#define DIALOG_DRAGGABLE 1 << 0 /* Allow moving by dragging the title bar*/
#define DIALOG_CENTERED 1 << 1  /* Always center on window resize */
#define DIALOG_TEXTINPUT 1 << 2 /* Hint SDL to read text input */
#define DIALOG_TOP_MOST 1 << 3  /* Draws background tint */
#define DIALOG_FLUID 1 << 4     /* Dialog adapts to window size. Includes DIALOG_CENTERED */

#define FLUID_BORDER 32

class sdl_helper;

class gui_element;

class dialog {
public:
	dialog(sdl_helper *sdl, SDL_Rect size, const std::string &title);

	dialog(sdl_helper *sdl, SDL_Point size, const std::string &title);

	virtual ~dialog();

	virtual void init();

	virtual void draw_background();

	virtual void draw_foreground();

	virtual void close();

	virtual bool handle_events(SDL_Event *event);

	virtual void action_performed(int8_t action_id);

	void add(gui_element *e);

	template<class T, typename... Args> T *add(Args &&... args)
	{
		auto id = m_screen_elements.size();
		auto elem = new T(std::forward<Args>(args)...);
		m_screen_elements.emplace_back(elem);
		if (elem->can_select())
			m_tab_items.emplace_back(elem);
		return elem;
	}

	void set_dimension(uint16_t w, uint16_t h);

	void set_flags(uint16_t flags);

	void reload_lang();

	void change_focus(int8_t id);

	SDL_Point position() const;

	sdl_helper *helper() const;

	int get_left() const;

	int get_top() const;

	int get_right() const;

	int get_bottom() const;

	int8_t get_next_id() const { return int8_t(m_screen_elements.size()); }

protected:
	/* Tab handling */
	gui_element *m_focused_element = nullptr;
	int8_t m_focused_element_id = 0;

	std::vector<std::unique_ptr<gui_element>> m_screen_elements;
	std::vector<gui_element *> m_tab_items;
	std::string m_title;

	int m_offset_x, m_offset_y{};
	bool m_is_dragging = false;
	uint16_t m_flags = 0x0000;

	sdl_helper *m_helper;
	SDL_Rect m_dimensions{};
	SDL_Rect m_title_bar{};
};
