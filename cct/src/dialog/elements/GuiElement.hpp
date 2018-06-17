/**
* Created by univrsal on 14.07.2017.
* This file is part of input-overlay which is licensed
* under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
* github.com/univrsal/input-overlay
*/

#pragma once

#include <SDL.h>

#define DEBUG_DRAW_OUTLINE 0

class Dialog;

class SDL_helper;

class GuiElement
{
public:
	virtual ~GuiElement();

	virtual void draw_background(void) = 0;

	virtual void draw_foreground(void);

	virtual void close(void);

	virtual bool handle_events(SDL_Event * event) = 0;

	virtual void resize(void);

	virtual bool is_mouse_over(const int &x, const int &y);

	virtual void set_pos(int x, int y);

	virtual uint8_t get_cursor(void);

	Dialog * get_parent(void);

	SDL_helper * get_helper();

	SDL_Rect * get_dimensions(void);

	void set_dim(SDL_Rect r);

	int8_t get_id(void);

	void init(Dialog *parent, SDL_Rect dim, int8_t id);

	void set_flags(uint8_t flags);

	int get_left(void);

	int get_top(void);

	int get_right(void);

	int get_bottom(void);

	int get_width(void);

	int get_height(void);
protected:
	Dialog * m_parent_dialog;
	SDL_Rect m_dimensions; /* Width/Height and absolute position */
	SDL_Point m_position; /* Position relative to dialog*/
	int8_t m_element_id;
	uint8_t m_flags = 0x0000;
};
