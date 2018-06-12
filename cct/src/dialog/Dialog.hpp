/**
 * Created by universallp on 23.05.2018.
 * This file is part of input-overlay which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "../util/SDL_helper.hpp"
#include "elements/GuiElement.hpp"
#include "elements/Label.hpp"
#include <string>
#include <vector>
#include <memory>

#define DIALOG_DRAGGABLE 1 << 0 /* Allow moving by dragging the title bar*/
#define DIALOG_CENTERED  1 << 1 /* Always center on window resize */
#define DIALOG_TEXTINPUT 1 << 2 /* Hint SDL to read text input */
#define DIALOG_TOP_MOST	 1 << 3 /* Draws background tint */
#define DIALOG_FLUID	 1 << 4 /* Dialog adapts to window size. Includes DIALOG_CENTERED */

#define FLUID_BORDER	 32

#define ACTION_FOCUSED -1
#define ACTION_UNFOCUSED -2
#define ACTION_OK -3
#define ACTION_CANCEL -4

class SDL_helper;

class GuiElement;

class Dialog
{
public:
	Dialog(SDL_helper * sdl, SDL_Rect size, std::string title);
	Dialog(SDL_helper * sdl, SDL_Point size, std::string title);

	virtual ~Dialog();

	virtual void init();

	virtual void draw_background(void);

	virtual void draw_foreground(void);

	virtual void close(void);

	virtual bool handle_events(SDL_Event * event);

	virtual void action_performed(int8_t action_id);

	void add(GuiElement * e);

	void set_flags(uint16_t flags);

	const SDL_Point position(void);

	SDL_helper * helper(void);

	int get_left(void);

	int get_top(void);

	int get_right(void);

	int get_bottom(void);
protected:
	std::vector<std::unique_ptr<GuiElement>> m_screen_elements;
	std::string m_title;

	int m_offset_x, m_offset_y;
	bool m_is_dragging = false;
	uint16_t m_flags = 0x00000000;

	SDL_helper * m_helper;
	SDL_Rect m_dimensions;
	SDL_Rect m_title_bar;
};
