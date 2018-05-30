/**
 * Created by universallp on 23.05.2018.
 * This file is part of input-overlay which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "util/SDL_helper.hpp"
#include "dialog/DialogSetup.hpp"
#include "dialog/DialogElementSettings.hpp"
#include "dialog/DialogHelp.hpp"
#include "Config.hpp"
#include <SDL.h>

class SDL_helper;
class Element;
class Config;
class DialogElementSettings;
class DialogHelp;

enum ToolState
{
	IN_SETUP,
	IN_BUILD,
	IN_HELP,
	IN_NEW_ELEMENT
};

#define TOOL_ACTION_HELP_OPEN 0
#define TOOL_ACTION_HELP_EXIT 1
#define TOOL_ACTION_NEW_ELEMENT_OPEN 2
#define TOOL_ACTION_NEW_ELEMENT_EXIT 3
#define TOOL_ACTION_NEW_ELEMENT_ADD 4

class Tool
{
public:
	Tool() { m_helper = nullptr; }
	Tool(SDL_helper * helper);
	~Tool();

	void program_loop();

	Element * get_selected(void);

	void action_performed(uint8_t type);
private:
	void close_toplevel(void);
	void handle_input();
	bool m_run_flag = true;
	bool m_queue_close = false; /* True when toplevel dialog should be closede */
	SDL_Event m_event;
	SDL_helper * m_helper;

	Config * m_config = nullptr;

	DialogSetup * m_setup_dialog = nullptr;
	DialogElementSettings * m_element_settings = nullptr;
	Dialog * m_toplevel = nullptr;
	ToolState m_state;
};
