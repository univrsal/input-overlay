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

#include <SDL.h>

#define DEBUG_DRAW_OUTLINE 0

class dialog;

class sdl_helper;

enum element_flags {
	TEXTBOX_NUMERIC = 1 << 0,
	TEXTBOX_HEX = 1 << 1,
	TEXTBOX_NO_SPACE = 1 << 2,
	TEXTBOX_KEYBIND = 1 << 3,
	TEXTBOX_DROP_FILE = 1 << 4,
	TEXTBOX_ALPHA_NUMERIC = 1 << 5,
	ELEMENT_FOCUSED = 1 << 6,
	ELEMENT_ENABLED = 1 << 7,
	/* Element will not align to parent dialog position */
	ELEMENT_ABSOLUTE_POSITION = 1 << 10,
	/* The element will not look up localized strings   */
	ELEMENT_UNLOCALIZED = 1 << 11
};

enum actions {
	ACTION_OK = -127,
	ACTION_CANCEL,
	ACTION_FOCUSED,
	ACTION_UNFOCUSED,
	ACTION_FILE_DROPPED,
	ACTION_COMBO_ITEM_SELECTED,
	ACTION_HELP_BUTTON,
	ACTION_NEW_ELEMENT,
	ACTION_SAVE_CONFIG,
	ACTION_DEL_ELEMENT,
	ACTION_MOD_ELEMENT,
	ACTION_TEXT_TYPED
};

class gui_element {
public:
	virtual ~gui_element();

	virtual void draw_background() = 0;

	virtual void draw_foreground();

	virtual void close();

	virtual bool handle_events(SDL_Event *event, bool was_handled) = 0;

	virtual bool can_select(); /* Tab selection */

	virtual void select_state(bool state);

	virtual void resize();

	virtual void refresh(); /* Update get_localization */

	virtual bool is_mouse_over(const SDL_Point *p = nullptr);

	virtual void set_pos(int x, int y);

	virtual uint8_t get_cursor();

	dialog *get_parent() const;

	sdl_helper *get_helper() const;

	SDL_Rect *get_dimensions();

	void set_dim(SDL_Rect r);

	int8_t get_id() const;

	void set_id(int8_t id) { m_element_id = id; }

	virtual void init(dialog *parent, SDL_Rect dim);

	void set_flags(uint16_t flags);

	int get_left() const;

	int get_top() const;

	int get_right() const;

	int get_bottom() const;

	int get_width() const;

	int get_height() const;

protected:
	dialog *m_parent_dialog = nullptr;
	bool m_enabled = true;
	SDL_Rect m_dimensions = {}; /* Width/Height and absolute position */
	SDL_Point m_position = {};  /* Position relative to dialog*/
	int8_t m_element_id = 0;
	uint16_t m_flags = 0x0000;
};
