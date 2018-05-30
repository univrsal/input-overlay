/**
 * Created by universal on 30.05.2018.
 * This file is part of reloded which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "Dialog.hpp"
#include "../util/SDL_helper.hpp"
#include "./elements/Button.hpp"
#include "../Tool.hpp"

class SDL_Helper;

class Tool;

class DialogHelp : public Dialog
{
public:
	DialogHelp(SDL_helper * sdl, SDL_Point size, std::string title, Tool * tool)
		: Dialog(sdl, size, title)
	{
		m_tool = tool;
	};

	void init();

	void action_performed(int8_t action_id);
private:
	Tool * m_tool = nullptr;

};
