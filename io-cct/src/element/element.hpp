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

#include "../../../io-obs/util/layout_constants.hpp"
#include <SDL.h>
#include <string>
#include <utility>

#define ELEMENT_HIDE_ALPHA 60

enum element_error {
	EE_VALID,
	EE_ID_NOT_UNIQUE,
	EE_ID_EMPTY,
	EE_TYPE_INVALID,
	EE_MAPPING_EMPTY,
	EE_KEYCODE_INVALID,
	EE_STICK_RADIUS,
	EE_MOUSE_RADIUS
};

class sdl_helper;

class notifier;

class dialog_element_settings;

class dialog_new_element;

class coordinate_system;

class ccl_config;

class texture;

/* Base class for display elements
 */
class element {
public:
	virtual void draw(texture *atlas, coordinate_system *cs, bool selected, bool alpha) = 0;

	virtual void write_to_file(ccl_config *cfg, SDL_Point *default_dim, uint8_t &layout_flags);

	virtual SDL_Rect *get_abs_dim(coordinate_system *cs);

	virtual void update_settings(dialog_new_element *dialog);

	virtual void update_settings(dialog_element_settings *dialog);

	virtual element_error is_valid(notifier *n, sdl_helper *h);

	void set_mapping(SDL_Rect r);

	void set_pos(int x, int y);

	void set_id(std::string id) { m_id = std::move(id); }

	void set_z_level(const uint8_t z) { m_z_level = z; }

	uint8_t get_z_level() const { return m_z_level; }

	std::string *get_id() { return &m_id; }

	int get_x() const { return m_position.x; }

	int get_y() const { return m_position.y; }

	int get_w() const { return m_mapping.w; }

	int get_h() const { return m_mapping.h; }

	int get_u() const { return m_mapping.x; }

	int get_v() const { return m_mapping.y; }

	virtual int get_vc() { return 0; }

	element_type get_type() const { return m_type; }

	SDL_Rect *get_mapping() { return &m_mapping; }

	virtual void handle_event(SDL_Event *event, sdl_helper *helper) = 0;

	/* Creates empty element and load settings from config */
	static element *read_from_file(ccl_config *file, const std::string &id, element_type t, SDL_Point *default_dim);

	/* Creates empty element and loads settings from dialog */
	static element *from_dialog(dialog_new_element *dialog);

	static bool valid_type(int t);

protected:
	element(); /* Used for creation over dialogs */
	element(element_type t, std::string id, SDL_Point pos, uint8_t z);

	static SDL_Rect read_mapping(ccl_config *file, const std::string &id, SDL_Point *default_dim);

	static SDL_Point read_position(ccl_config *file, const std::string &id);

	static uint8_t read_layer(ccl_config *file, const std::string &id);

	static element_side read_side(ccl_config *file, const std::string &id);

	element_type m_type;
	SDL_Point m_position{}; /* Final position in overlay */
	SDL_Rect m_mapping{};   /* texture mappings */
	SDL_Rect m_dimensions_scaled{};

	uint8_t m_scale = 0; /* Currently used scale factor */

	uint8_t m_z_level = 0; /* Determines draw and selection order */

	std::string m_id;
};
