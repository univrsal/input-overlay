/**
 * Created by univrsal on 23.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "dialog.hpp"
#include "../util/sdl_helper.hpp"
#include "elements/textbox.hpp"
#include "elements/button.hpp"
#include "elements/combobox.hpp"
#include "../util/constants.hpp"

class tool;

class sdl_helper;

class notifier;

class dialog_setup : public dialog {
public:
	dialog_setup(sdl_helper *sdl, notifier *notifier, tool *t)
		: dialog(sdl, SDL_Point{500, 280}, LANG_DIALOG_SETUP)
	{
		m_notifier = notifier;
		m_tool = t;
	};

	void init() override;

	void draw_background() override;

	void action_performed(int8_t action_id) override;

	SDL_Point get_default_dim() const;

	bool should_load_cfg() const;

	const char *get_config_path() const;

	const char *get_texture_path() const;

private:
	notifier *m_notifier = nullptr;

	bool m_load_cfg = false;

	combobox *m_lang_box = nullptr;
	textbox *m_texture_path = nullptr;
	textbox *m_config_path = nullptr;
	textbox *m_def_w = nullptr;
	textbox *m_def_h = nullptr;
	textbox *m_h_space = nullptr;
	textbox *m_v_space = nullptr;

	tool *m_tool = nullptr;
public:
	SDL_Point get_rulers() const;
};
