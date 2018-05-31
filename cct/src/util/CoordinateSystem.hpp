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

#define X_AXIS 90
#define Y_AXIS 70

class CoordinateSystem
{
public:
	CoordinateSystem(SDL_Point origin, SDL_Rect size, SDL_helper * h)
	{
		m_helper = h;
		m_origin.x = origin.x + size.x;
		m_origin.y = origin.x + size.y;
		m_origin_anchor = m_origin;
		m_dimensions = size;
		m_scale_f = 1;
	}

	bool handle_events(SDL_Event * e);

	void draw_foreground(void);

	void draw_background(void);

	int get_origin_left(void) { return m_dimensions.x + m_origin_anchor.x; }
	int get_origin_top(void) { return m_dimensions.y + m_origin_anchor.y; }

	int get_bottom(void) { return m_dimensions.x + m_dimensions.h; }
	int get_right(void) { return m_dimensions.x + m_dimensions.w; }
private:
	uint8_t m_scale_f;
	SDL_Rect m_dimensions;
	SDL_Point m_origin_anchor;
	SDL_Point m_origin;
	SDL_Point m_drag_offset;
	SDL_helper * m_helper = nullptr;
};
