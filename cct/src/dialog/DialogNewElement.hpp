/**
 * Created by universallp on 30.05.2018.
 * This file is part of input-overlay which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "Dialog.hpp"
#include "elements/Button.hpp"
#include "elements/Textbox.hpp"
#include "elements/AtlasSelector.hpp"
#include "../Tool.hpp"
#include "../util/SDL_helper.hpp"
#include "../util/Texture.hpp"
#include "../util/CoordinateSystem.hpp"

class SDL_helper;

class Texture;

class Tool;

enum ElementType;

class DialogNewElement : public Dialog
{
public:
	DialogNewElement(SDL_helper * sdl, SDL_Point size, std::string title, Tool * tool, ElementType type)
		: Dialog(sdl, size, title)
	{
		m_tool = tool;
		m_type = type;
	}

	void init();

	void action_performed(int8_t action_id);

	void draw_background(void);

	void draw_foreground(void);

	bool handle_events(SDL_Event * event);
private:
	/* Adds default elements*/
	void add_selection_elements(void);

	/* Adds key code elements */
	void add_keycode_elements(void);

	/* Y position of the lowest element */
	uint16_t m_element_y = 0;
	uint16_t m_id = 1;

	AtlasSelector * m_selector = nullptr;
	Tool * m_tool = nullptr;
	ElementType m_type;

	Textbox * m_w = nullptr;
	Textbox * m_h = nullptr;
	Textbox * m_u = nullptr;
	Textbox * m_v = nullptr;
	Textbox * m_x = nullptr;
	Textbox * m_y = nullptr;

	Textbox * m_keycode = nullptr;
};
