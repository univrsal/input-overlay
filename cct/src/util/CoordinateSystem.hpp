/**
 * Created by universal on 30.05.2018.
 * This file is part of reloded which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <SDL.h>
#include "SDL_helper.hpp"

class SDL_helper;

class CoordinateSystem
{
public:
	CoordinateSystem()
	{
		m_scale_f = 1;
		m_origin = { 0, 0 };
		m_origin_anchor = m_origin;
		m_dimensions = { 0, 0, 1280, 720 };
	}

	CoordinateSystem(SDL_Point origin, SDL_Rect size, SDL_helper * h)
	{
		m_helper = h;
		m_origin.x = origin.x + size.x;
		m_origin.y = origin.x + size.y;
		m_origin_anchor = m_origin;
		m_dimensions = size;
		m_scale_f = 1;
	}

	bool crop_rect(SDL_Rect & mapping, SDL_Rect& destination);

	bool handle_events(SDL_Event * e);

	void draw_foreground(void);

	void draw_background(void);

	int get_origin_left(void) { return m_dimensions.x + m_origin_anchor.x; }
	int get_origin_top(void) { return m_dimensions.y + m_origin_anchor.y; }

	SDL_Point * get_origin(void) { return &m_origin; }

	int get_bottom(void) { return m_dimensions.x + m_dimensions.h; }
	int get_right(void) { return m_dimensions.x + m_dimensions.w; }

	int get_scale(void) { return m_scale_f; }
	SDL_Rect get_dimensions(void) { return m_dimensions; }
	SDL_helper * get_helper(void) { return m_helper; }

private:
	uint8_t m_scale_f;
	SDL_Rect m_dimensions;
	SDL_Point m_origin_anchor;
	SDL_Point m_origin;
	SDL_Point m_drag_offset;
	SDL_helper * m_helper = nullptr;
};
