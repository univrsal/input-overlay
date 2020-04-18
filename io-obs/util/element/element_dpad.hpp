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

#include "element_texture.hpp"

class element_data_dpad : public element_data {
public:
	/*
        Separate constructors are used on linux
        because the values can't be queried together
    */

	/* Xinput directly generates direction */
	element_data_dpad(dpad_direction a, dpad_direction b);

	element_data_dpad(dpad_direction d = DD_LEFT, button_state state = BS_RELEASED);

	bool is_persistent() override;

	bool merge(element_data *other) override;

	dpad_texture get_direction() const;

	button_state get_state() const;

private:
	uint16_t m_direction;
	button_state m_state;
};

class element_dpad : public element_texture {
public:
	element_dpad();

	void load(const QJsonObject &obj) override;

	void draw(gs_effect_t *effect, gs_image_file_t *image, element_data *data,
			  sources::overlay_settings *settings) override;

	data_source get_source() override;

private:
	/* Center is in m_mapping */
	gs_rect m_mappings[8]; /* Left, Right, Up, Down, Top Left, Top Right, Bottom Left, Bottom Right */
};
