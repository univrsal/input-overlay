/**
 * Created by universallp on 27.05.2018.
 * This file is part of input-overlay which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "Dialog.hpp"
#include "../util/SDL_helper.hpp"
#include "./elements/Textbox.hpp"
#include "./elements/Button.hpp"
#include "../Tool.hpp"

#define ACTION_HELP_BUTTON -10
#define ACTION_NEW_ELEMENT -11
#define ACTION_SAVE_CONFIG -12
#define ACTION_DEL_ELEMENT -13

class SDL_helper;

class Tool;

class DialogElementSettings : public Dialog
{
public:
	DialogElementSettings(SDL_helper * sdl, SDL_Rect size, std::string title, Tool * tool)
		: Dialog(sdl, size, title)
	{
		m_tool = tool;
	};

	void init();

	void action_performed(int8_t action_id);

	void set_dimensions(int w, int h);
	void set_position(int x, int y);
	void set_uv(int u, int v);
private:
	bool m_finished = false;

	Textbox * m_element_id = nullptr;
	Textbox * m_element_width = nullptr;
	Textbox * m_element_height = nullptr;
	Textbox * m_element_x = nullptr;
	Textbox * m_element_y = nullptr;
	Textbox * m_element_u = nullptr;
	Textbox * m_element_v = nullptr;

	Tool * m_tool = nullptr;
};
