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

class SDL_helper;

class DialogElementSettings : public Dialog
{
public:
	DialogElementSettings(SDL_helper * sdl, SDL_Point size, std::string title)
		: Dialog(sdl, size, title)
	{
		/* NO-OP */
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

};
