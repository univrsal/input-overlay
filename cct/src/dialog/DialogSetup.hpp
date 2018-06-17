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

class SDL_helper;

class DialogSetup : public Dialog
{
public:
	DialogSetup(SDL_helper * sdl, SDL_Point size)
		: Dialog(sdl, size, "Overlay setup")
	{
		/* NO-OP */
	};
	void init();

	void action_performed(int8_t action_id);

	bool is_finished(void);

	const std::string * get_config_path(void);

	const std::string * get_texture_path(void);
private:
	bool m_finished = false;
	Textbox * m_texture_path = nullptr;
	Textbox * m_config_path = nullptr;
};
