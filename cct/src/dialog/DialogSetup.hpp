/**
 * Created by universallp on 23.05.2018.
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

class DialogSetup : public Dialog
{
public:
	DialogSetup(SDL_helper * sdl, SDL_Point size, std::string title)
		: Dialog(sdl, size, title)
	{
		/* NO-OP */
	};
	void init();

	void action_performed(int8_t action_id);

private:

};
