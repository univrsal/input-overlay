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
	Element(ElementType t, SDL_Point pos, SDL_Rect map, uint16_t keycode)
	{
		m_type = t;
		m_pos = pos;
		m_texture_mapping = map;
		m_keycode = keycode;
	}

	SDL_Rect get_abs_dim(CoordinateSystem * cs)
	{
		SDL_Rect temp = { m_pos.x * cs->get_scale() + cs->get_origin()->x,
			m_pos.y * cs->get_scale() + cs->get_origin()->y,
			m_texture_mapping.w * cs->get_scale(), m_texture_mapping.h * cs->get_scale() };
		return temp;
	}

	void draw(Texture * atlas, CoordinateSystem * cs, bool selected)
	{
		SDL_Rect temp = { m_pos.x * cs->get_scale() + cs->get_origin()->x,
			m_pos.y * cs->get_scale() + cs->get_origin()->y,
			m_texture_mapping.w * cs->get_scale(),
			m_texture_mapping.h * cs->get_scale()};
		SDL_Rect temp_mapping = m_texture_mapping;

		//bool result = (cs->crop_rect(temp_mapping, temp));
		
		//if (!result)
		//	return;

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

	int get_x() { return m_pos.x; }
	int get_y() { return m_pos.y; }
	int get_w() { return m_texture_mapping.w; }
	int get_h() { return m_texture_mapping.h; }
	int get_u() { return m_texture_mapping.x; }
	int get_v() { return m_texture_mapping.y; }
private:
	ElementType m_type;
	SDL_Point m_pos; /* Final position in overlay */
	SDL_Rect m_texture_mapping; /* Position in texture*/
	uint16_t m_keycode;
};

class Config
{
public:
	Config(const std::string * texture, const std::string * config, SDL_helper * h, DialogElementSettings * s);
	~Config();

	void draw_elements(void);

	void handle_events(SDL_Event * e);

	Element * m_selected = nullptr;
	const std::string * m_texture_path;
	const std::string * m_config_path;
	std::vector<std::unique_ptr<Element>> m_elements;

	Texture * get_texture(void);
private:
	CoordinateSystem m_cs;

	SDL_helper * m_helper = nullptr;
	Texture * m_atlas = nullptr;
	DialogElementSettings * m_settings = nullptr;

	bool m_dragging_element = false;
	SDL_Point m_drag_element_offset;
};
