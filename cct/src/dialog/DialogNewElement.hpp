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
#include "../Tool.hpp"
#include "../util/SDL_helper.hpp"
#include "../util/CoordinateSystem.hpp"

class SDL_helper;

class Tool;

class CoordinateSystem;

class DialogNewElement : public Dialog
{
public:
	DialogNewElement(SDL_helper * sdl, SDL_Point size, std::string title, Tool * tool)
		: Dialog(sdl, size, title)
	{
		
	}

	void close(void);

	void init();

	void action_performed(int8_t action_id);

	void draw_background(void);

	void draw_foreground(void);

	bool handle_events(SDL_Event * event);
private:
	Tool * m_tool = nullptr;
	CoordinateSystem * m_cs = nullptr;
};
