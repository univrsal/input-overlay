/**
 * Created by universal on 27.05.2018.
 * This file is part of reloded which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "util/SDL_helper.hpp"
#include "util/Texture.hpp"
#include "dialog/DialogElementSettings.hpp"
#include <string>
#include <SDL.h>
#include <memory>
#include <vector>

class DialogElementSettings;

class SDL_helper;

class Texture;

enum ElementType
{
	TEXTURE,
	BUTTON_KEYBOARD,
	BUTTON_GAMEPAD,
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

	SDL_Rect get_abs_dim(int scale, SDL_Point * o)
	{
		SDL_Rect temp = { m_pos.x * scale + o->x, m_pos.y * scale + o->y, m_texture_mapping.w * scale, m_texture_mapping.h * scale };
		return temp;
	}

	void draw(SDL_helper * h, Texture * atlas, const SDL_Point * origin, const SDL_Rect * bounds, int scale, bool selected)
	{
		SDL_Rect temp = { m_pos.x * scale + origin->x,
			m_pos.y * scale + origin->y,
			m_texture_mapping.w * scale,
			m_texture_mapping.h * scale };
		SDL_Rect temp_mapping = m_texture_mapping;
		
		/*
			Boundary cropping
			If the element is partially or fully
			out of the coordinate system
			it'll be cropped to only show the part
			that's inside the coordinate system.
		*/

		if (temp.x < bounds->x)
		{
			if (temp.x + temp.w < bounds->x) /* It's fully out of bounds */
				return;
			/* Crop it so it ends at the x axis */
			temp_mapping.w -= (bounds->x -temp.x) / scale;
			temp.w = temp_mapping.w * scale;
			
			temp_mapping.x += (bounds->x - temp.x) / scale;
			temp.x += bounds->x - temp.x;
		}

		if (temp.x - temp.w > bounds->x + bounds->w)
		{
			if (temp.x > bounds->x + bounds->w)
				return;
		}

		if (temp.y < bounds->y)
		{
			if (temp.y + temp.h < bounds->y)
				return;
			temp_mapping.h -= (bounds->y - temp.y) / scale;
			temp.h = temp_mapping.h * scale;

			temp_mapping.y += (bounds->y - temp.y) / scale;
			temp.y += bounds->y - temp.y;
		}

		if (temp.y > bounds->y + bounds->w)
		{

		}

		atlas->draw(h->renderer(), &temp, &temp_mapping);

		if (selected)
		{
			h->util_draw_rect(&temp, h->palette()->red());
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
private:
	SDL_helper * m_helper = nullptr;
	Texture * m_atlas = nullptr;
	DialogElementSettings * m_settings = nullptr;
	SDL_Point m_origin;

	int m_scale_f = 1;

	bool m_dragging_all = false;
	bool m_dragging_element = false;
	SDL_Point m_drag_offset;
	SDL_Point m_drag_element_offset;
};
