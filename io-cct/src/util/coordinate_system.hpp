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

#include "sdl_helper.hpp"
#include <SDL.h>

#define SIZE_LEFT 0
#define SIZE_RIGHT 1
#define SIZE_TOP 2
#define SIZE_BOTTOM 3
#define SIZE_MOVE 4
#define SIZE_NONE 5

class sdl_helper;

class coordinate_system {
public:
    coordinate_system()
    {
        m_scale_f = 1;
        m_origin = { 0, 0 };
        m_origin_anchor = m_origin;
        m_dimensions = { 0, 0, 1280, 720 };
        m_system_area = { get_origin_left(), get_origin_top(), m_dimensions.w - m_origin_anchor.x,
            m_dimensions.h - m_origin_anchor.y };
    }

    coordinate_system(const SDL_Point origin, const SDL_Rect size, sdl_helper* h)
    {
        m_helper = h;
        m_origin.x = origin.x + size.x;
        m_origin.y = origin.y + size.y;
        m_origin_anchor = origin;
        m_dimensions = size;
        m_scale_f = 1;
        m_system_area = { get_origin_left(), get_origin_top(), m_dimensions.w - m_origin_anchor.x,
            m_dimensions.h - m_origin_anchor.y };
    }

    void enable_border()
    {
        m_border = true;
    }

    void enable_crosshair()
    {
        m_crosshair = true;
    }

    bool handle_events(SDL_Event* e);

    void draw_foreground() const;

    void draw_background() const;

    void set_dimensions(SDL_Rect r);

    void set_pos(int x, int y);

    void set_grid_space(const SDL_Point p)
    {
        m_grid_spacing = p;
        m_has_custom_grid = true;
    }

    void set_ruler_offset(const SDL_Point p)
    {
        m_ruler_offset = p;
        m_has_rulers = true;
    }

    /* Adjusts to pixel raster */
    int adjust(int i) const;

    /* X position of Origin anchor */
    int get_origin_left() const
    {
        return m_dimensions.x + m_origin_anchor.x;
    }

    /* Y position of Origin anchor */
    int get_origin_top() const
    {
        return m_dimensions.y + m_origin_anchor.y;
    }

    SDL_Point* get_origin()
    {
        return &m_origin;
    }

    int get_bottom() const
    {
        return m_dimensions.y + m_dimensions.h;
    }

    int get_right() const
    {
        return m_dimensions.x + m_dimensions.w;
    }

    int get_left() const
    {
        return m_dimensions.x;
    }

    int get_top() const
    {
        return m_dimensions.y;
    }

    int get_width() const
    {
        return m_dimensions.w;
    }

    int get_height() const
    {
        return m_dimensions.h;
    }

    /* Returns origin adjusted to viewport */
    int get_origin_x() const
    {
        return m_origin.x - get_left() - m_origin_anchor.x;
    }

    /* Returns origin adjusted to viewport */
    int get_origin_y() const
    {
        return m_origin.y - get_top() - m_origin_anchor.y;
    }

    int get_scale() const
    {
        return m_scale_f;
    }

    SDL_Rect get_dimensions() const
    {
        return m_dimensions;
    }

    sdl_helper* get_helper() const
    {
        return m_helper;
    }

    const SDL_Rect* get_system_area() const
    {
        return &m_system_area;
    }

    void set_selection(SDL_Rect* r)
    {
        m_selection = r;
    }

    /* Prepares the viewport to fit inside the coordinate system area */
    void begin_draw() const
    {
        SDL_RenderSetViewport(m_helper->renderer(), &m_system_area);
    }

    /* Resets the viewport */
    void end_draw() const
    {
        SDL_RenderSetViewport(m_helper->renderer(), nullptr);
    }

    /* Translates any point to the viewport*/
    void translate(int& x, int& y) const;

    void draw_selection() const;

    bool changing_selection() const
    {
        return m_sizing || m_dragging || m_selecting;
    }

private:
    void draw_rulers() const;

    void mouse_state(SDL_Event* event);

    static bool in_range(const int a, const int b, const int range)
    {
        return a <= b + range && a >= b - range;
    }

    bool in_between(const int a, const int min, const int max, const int o) const
    {
        return a >= min * m_scale_f + o && a <= max * m_scale_f + o;
    }

    int m_scale_f = 1; /* Multiplier for zooming */
    uint8_t m_size_mode = SIZE_NONE;

    SDL_Rect m_dimensions = {}; /* Complete size of the system */
    SDL_Rect m_system_area = {}; /* Area minus axes and scale text */
    SDL_Rect* m_selection = nullptr; /* For atlas selector */

    SDL_Point m_selection_a = {};
    SDL_Point m_origin_anchor = {}; /* Constant position of the origin */
    SDL_Point m_origin = {}; /* Origin after zooming and moving */
    SDL_Point m_drag_offset = {};
    SDL_Point m_grid_spacing = {};
    SDL_Point m_ruler_offset = {};

    sdl_helper* m_helper = nullptr;

    bool m_selecting = false;
    bool m_sizing = false;
    bool m_has_custom_grid = false;
    bool m_has_rulers = false;

    bool m_dragging = false;
    bool m_border = false; /* Used inside dialogs */
    bool m_crosshair = false;
    bool m_mouse_over = false;
};
