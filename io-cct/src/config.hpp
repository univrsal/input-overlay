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

#include "util/coordinate_system.hpp"
#include "element/element.hpp"
#include <string>
#include <SDL.h>
#include <memory>
#include <vector>

class ccl_config;

class notifier;

class coordinate_system;

class dialog_element_settings;

class sdl_helper;

class texture;

class config {
public:
	config(const char *texture_path, const char *config, SDL_Point def_dim, SDL_Point space, sdl_helper *h,
		   dialog_element_settings *s);

	~config();

	void draw_elements();

	void handle_events(SDL_Event *e);

	std::string m_texture_path;
	std::string m_config_path;

	void write_config(notifier *n);

	void read_config(notifier *n);

	texture *get_texture() const;

	SDL_Point get_default_dim() const;

	void queue_delete(uint16_t id)
	{
		if (!m_elements.empty())
			m_element_to_delete = id;
	}

	std::vector<std::unique_ptr<element>> m_elements;

	element *selected() const { return m_selected; }

	uint16_t selected_id() const { return m_selected_id; }

	void reset_selection();

private:
	/* Move selected elements*/
	void move_elements(int new_x, int new_y);

	inline void move_element(int mouse_x, int mouse_y);

	static inline bool is_rect_in_rect(const SDL_Rect *a, const SDL_Rect *b);

	int16_t m_element_to_delete = -1;
	int16_t m_selected_id = -1;
	element *m_selected = nullptr;

	coordinate_system m_cs;

	sdl_helper *m_helper = nullptr;
	texture *m_atlas = nullptr;
	dialog_element_settings *m_settings = nullptr;

	bool m_in_single_selection = false; /* Flag for dragging single element */
	SDL_Point m_drag_offset{};

	SDL_Point m_default_dim{};
	SDL_Point m_offset{};

	/* Selection stuff */
	std::vector<uint16_t> m_selected_elements;
	SDL_Rect m_total_selection{}; /* Rectangle fitting all selected elements, oriented to origin */
	SDL_Rect m_temp_selection{};  /* Indicator of current rectangle selected by mouse */
	SDL_Point m_selection_start{};

	bool m_in_multi_selection = false;
	bool m_dragging_elements = false; /* Flag for dragging entire selection*/
};
