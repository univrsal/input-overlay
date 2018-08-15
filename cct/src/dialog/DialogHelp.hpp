/**
 * Created by universal on 30.05.2018.
 * This file is part of reloded which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "Dialog.hpp"
#include "../util/SDL_helper.hpp"
#include "../util/Constants.hpp"
#include "./elements/Button.hpp"
#include "../Tool.hpp"

class SDL_Helper;

class Tool;

class DialogHelp : public Dialog
{
public:
	DialogHelp(SDL_helper * sdl, Tool * tool)
		: Dialog(sdl, SDL_Point { 355, 420 }, LANG_DIALOG_HELP)
	{
		m_tool = tool;
	};

	void init() override;

	void action_performed(int8_t action_id) override;
private:
	Tool * m_tool = nullptr;

};
