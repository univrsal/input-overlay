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

#include "../log.h"
#include "element_texture.hpp"
#include <layout_constants.h>
#include <netlib.h>

class element_data_button : public element_data {
public:
	element_data_button(const button_state state = BS_RELEASED) : element_data(ET_BUTTON)
	{
		m_state = state;
		bdebug("state: %i", static_cast<bool>(state));
	}

	button_state get_state() const { return m_state; }

	bool merge(element_data *other) override;

private:
	button_state m_state;
};

class element_button : public element_texture {
public:
	element_button() : element_texture(ET_BUTTON), m_pressed() {}

	void load(const QJsonObject &objc) override;

	void draw(gs_effect_t *effect, gs_image_file_t *image, element_data *data,
			  sources::overlay_settings *settings) override;

	data_source get_source() override { return is_gamepad ? DS_GAMEPAD : DS_DEFAULT; }

private:
	bool is_gamepad = false;
	gs_rect m_pressed;
};
