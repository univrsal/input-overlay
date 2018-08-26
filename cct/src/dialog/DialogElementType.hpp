/**
 * Created by univrsal on 15.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include "Dialog.hpp"
#include "../util/SDL_Helper.hpp"

class SDL_Helper;

class Tool;

class Combobox;

class DialogElementType : public Dialog
{
public:
	DialogElementType(SDL_Helper * sdl, Tool * tool);
	void init() override;
	void action_performed(int8_t action_id) override;
private:
	Tool * m_tool;
	Combobox * m_type = nullptr;
};
