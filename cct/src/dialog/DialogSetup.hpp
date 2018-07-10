/**
 * Created by univrsal on 23.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "Dialog.hpp"
#include "../util/SDL_helper.hpp"
#include "elements/Textbox.hpp"
#include "elements/Button.hpp"
#include "../../../ccl/ccl.hpp"

class Tool;

class SDL_helper;

class Notifier;

class DialogSetup : public Dialog
{
public:
	DialogSetup(SDL_helper * sdl, SDL_Point size, Notifier * notifier, Tool * t)
		: Dialog(sdl, size, "Overlay setup")
	{
		m_notifier = notifier;
		m_tool = t;
	};

	void init();

	void action_performed(int8_t action_id);

	SDL_Point get_default_dim();

	bool should_load_cfg(void);

	const char * get_config_path(void);

	const char * get_texture_path(void);
private:
	Notifier * m_notifier = nullptr;

	bool m_load_cfg = false;

	Textbox * m_texture_path = nullptr;
	Textbox * m_config_path = nullptr;
	Textbox * m_def_w = nullptr;
	Textbox * m_def_h = nullptr;

	Tool * m_tool = nullptr;
};
