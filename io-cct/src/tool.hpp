/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <universailp@web.de>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#pragma once

#include "element/element.hpp"
#include <layout_constants.h>
#include <SDL.h>

class dialog;

class notifier;

class sdl_helper;

class element;

class config;

class dialog_element_settings;

class DialogHelp;

class dialog_new_element;

class texture;

enum tool_state { IN_SETUP, IN_BUILD, IN_HELP, IN_ELEMENT_TYPE, IN_NEW_ELEMENT };

#define TOOL_ACTION_NEW_ELEMENT_ADD 4
#define TOOL_ACTION_MOD_ELEMENT_APPLY 6
#define TOOL_ACTION_SAVE_CONFIG 7
#define TOOL_ACTION_SETUP_EXIT 8

enum dialog_id { NONE, HELP, NEW_ELEMENT, MOD_ELEMENT, SELECECT_TYPE };

class tool {
public:
	tool() { m_helper = nullptr; }

	tool(sdl_helper *helper, const char *texture, const char *config);

	~tool();

	void program_loop();

	element *get_selected() const;

	uint16_t get_selected_id() const;

	void action_performed(uint8_t type);

	texture *get_atlas() const;

	void delete_element(uint16_t id) const;

	void set_new_element_type(const element_type type) { m_new_element_type = type; }

	void queue_dialog_open(dialog_id id);

	void queue_dialog_close();

	bool element_id_unique(const char *id) const;

	element_error verify_element(dialog_new_element *d, bool modify_mode) const;

	const char *get_texture_path() const { return m_texture_path; }

	const char *get_config_path() const { return m_config_path; }

private:
	void add_element(element *e) const;

	void close_top_level();

	void handle_input();

	bool m_run_flag = true;

	bool m_queue_close = false; /* True when top level dialog should be closed */
	dialog_id m_queued_dialog = NONE;

	/* Stores the type, which the user selected for the next element */
	element_type m_new_element_type = ET_INVALID;

	SDL_Event m_event;
	sdl_helper *m_helper;

	config *m_config = nullptr;

	dialog_element_settings *m_element_settings = nullptr;
	dialog *m_toplevel = nullptr;
	notifier *m_notify = nullptr;
	tool_state m_state;

	const char *m_texture_path = "";
	const char *m_config_path = "";
};
