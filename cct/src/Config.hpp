/**
 * Created by universal on 27.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "util/SDL_helper.hpp"
#include "util/Texture.hpp"
#include "util/CoordinateSystem.hpp"
#include <string>
#include <SDL.h>
#include <memory>
#include <vector>

class CoordinateSystem;

class DialogElementSettings;

class SDL_helper;

class Texture;

enum ElementType
{
	TEXTURE,
	BUTTON_KEYBOARD,
	BUTTON_GAMEPAD,
	BUTTON_MOUSE,
	TRIGGER_GAMEPAD,
	MOUSE_MOVEMENT,
	ANALOG_STICK,
	TEXT,
	DPAD,
	DPAD_STICK
};

class Element
{
public:
	Element(ElementType t, std::string id, SDL_Point pos, SDL_Rect map, uint16_t keycode)
	{
		m_type = t;
		m_pos = pos;
		m_texture_mapping = map;
		m_keycode = keycode;
		m_id = id;
	}

	SDL_Rect get_abs_dim(CoordinateSystem * cs)
	{
		SDL_Rect temp = { m_pos.x * cs->get_scale() + cs->get_origin()->x,
			m_pos.y * cs->get_scale() + cs->get_origin()->y,
			m_texture_mapping.w * cs->get_scale(), m_texture_mapping.h * cs->get_scale() };
		return temp;
	}

	SDL_Rect get_dim()
	{
		return SDL_Rect { m_pos.x, m_pos.y, m_texture_mapping.w, m_texture_mapping.h };
	}

	void draw(Texture * atlas, CoordinateSystem * cs, bool selected)
	{
		SDL_Rect temp = { m_pos.x * cs->get_scale() + cs->get_origin_x(),
			m_pos.y * cs->get_scale() + cs->get_origin_y(),
			m_texture_mapping.w * cs->get_scale(),
			m_texture_mapping.h * cs->get_scale()};
		SDL_Rect temp_mapping = m_texture_mapping;

		if (m_pressed)
			temp_mapping.y += temp_mapping.h + 3;

		atlas->draw(cs->get_helper()->renderer(), &temp, &temp_mapping);

		if (selected)
		{
			cs->get_helper()->util_draw_rect(&temp, cs->get_helper()->palette()->red());
		}
	}

	void set_pos(int x, int y)
	{
		m_pos.x = x;
		m_pos.y = y;
	}

	void set_uv(int u, int v)
	{
		m_texture_mapping.x = u;
		m_texture_mapping.y = v;
	}

	void set_dim(int w, int h)
	{
		m_texture_mapping.w = w;
		m_texture_mapping.h = h;
	}

	void set_map(SDL_Rect map)
	{
		m_texture_mapping = map;
	}

	void set_id(std::string id) { m_id = id; }
	void set_vc(uint16_t vc){ m_keycode = vc; }

	std::string * get_id() { return &m_id; }
	int get_x() { return m_pos.x; }
	int get_y() { return m_pos.y; }
	int get_w() { return m_texture_mapping.w; }
	int get_h() { return m_texture_mapping.h; }
	int get_u() { return m_texture_mapping.x; }
	int get_v() { return m_texture_mapping.y; }

	uint16_t get_vc(void) { return m_keycode; }
	void set_pressed(bool b) { m_pressed = b; }

	const SDL_Rect * get_mapping(void) { return &m_texture_mapping; }
private:
	ElementType m_type;
	SDL_Point m_pos; /* Final position in overlay */
	SDL_Rect m_texture_mapping; /* Position in texture*/
	uint16_t m_keycode;
	std::string m_id;

	bool m_pressed = false; /* used to highlight in preview */
};

class Config
{
public:
	Config(const std::string * texture, const std::string * config, SDL_Point def_dim, SDL_helper * h, DialogElementSettings * s);
	~Config();

	void draw_elements(void);

	void handle_events(SDL_Event * e);

	const std::string * m_texture_path;
	const std::string * m_config_path;
	
	Texture * get_texture(void);

	SDL_Point get_default_dim(void);

	void queue_delete(uint16_t id) { m_element_to_delete = id; }

	std::vector<std::unique_ptr<Element>> m_elements;

	Element * selected() { return m_selected; }
	uint16_t selecte_id() { return m_selected_id; }

	void reset_selected_element(void);
private:
	/* Move selected elements*/
	void move_elements(int new_x, int new_y);

	inline bool is_rect_in_rect(const SDL_Rect * a, const SDL_Rect * b);

	int16_t m_element_to_delete = -1;
	uint16_t m_selected_id = 0;
	Element * m_selected = nullptr;

	CoordinateSystem m_cs;

	SDL_helper * m_helper = nullptr;
	Texture * m_atlas = nullptr;
	DialogElementSettings * m_settings = nullptr;

	bool m_dragging_element = false; /* Flag for dragging single element */
	SDL_Point m_drag_offset;

	SDL_Point m_default_dim;

	/* Selection stuff */
	std::vector<uint16_t> m_selected_elements;
	SDL_Rect m_total_selection; /* Rectangle fitting all selected elements, oriented to origin */
	SDL_Rect m_temp_selection; /* Indicator of current rectangle selected by mouse */
	SDL_Point m_selection_start;

	bool m_selecting = false;
	bool m_dragging_elements = false; /* Flag for dragging entire selection*/
};
