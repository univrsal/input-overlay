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

#include "coordinate_system.hpp"
#include "../util/palette.hpp"
#include "constants.hpp"

bool coordinate_system::handle_events(SDL_Event *e)
{
	auto was_handled = false;

	if (e->type == SDL_MOUSEBUTTONDOWN) {
		if (e->button.button == SDL_BUTTON_RIGHT) {
			if (m_mouse_over) {
				m_drag_offset = {e->button.x - m_origin.x, e->button.y - m_origin.y};
				m_dragging = true;
				was_handled = true;
			}
		} else if (e->button.button == SDL_BUTTON_LEFT && m_selection) {
			if (m_mouse_over) {
				if (SDL_RectEmpty(m_selection)) {
					m_selection_a.x = e->button.x;
					m_selection_a.y = e->button.y;
					m_selecting = true;
					was_handled = true;
				} else if (m_size_mode != SIZE_NONE) {
					m_sizing = true;
					was_handled = true;
					if (m_size_mode != SIZE_MOVE) {
						m_selection_a = {m_selection->x + m_selection->w, m_selection->y + m_selection->h};
					} else {
						m_selection_a = {e->button.x - m_selection->x * m_scale_f,
										 e->button.y - m_selection->y * m_scale_f};
					}
				}
			}
		}
	} else if (e->type == SDL_MOUSEMOTION) {
		m_mouse_over = m_helper->util_mouse_in_rect(&m_system_area);

		if (m_dragging && (e->motion.state & SDL_BUTTON_RMASK)) {
			m_origin.x = UTIL_MIN(e->button.x - m_drag_offset.x, get_origin_left());
			m_origin.y = UTIL_MIN(e->button.y - m_drag_offset.y, get_origin_top());
			was_handled = true;
		} else if (m_selecting && (e->motion.state & SDL_BUTTON_LMASK)) {
			m_selection->x = UTIL_MIN(e->button.x, m_selection_a.x);
			m_selection->y = UTIL_MIN(e->button.y, m_selection_a.y);

			translate(m_selection->x, m_selection->y);

			m_selection->x = ceil(UTIL_MAX((m_selection->x - get_origin_x()) / ((float)m_scale_f), 0));
			m_selection->y = ceil(UTIL_MAX((m_selection->y - get_origin_y()) / ((float)m_scale_f), 0));

			m_selection->w = ceil(SDL_abs(m_selection_a.x - e->button.x) / static_cast<float>(m_scale_f));
			m_selection->h = ceil(SDL_abs(m_selection_a.y - e->button.y) / static_cast<float>(m_scale_f));
		} else if (m_sizing && (e->motion.state & SDL_BUTTON_LMASK)) {
			switch (m_size_mode) {
			case SIZE_RIGHT:
				m_selection->w = UTIL_MAX(round((e->button.x - m_origin.x) / ((float)m_scale_f) - m_selection->x), 4);
				break;
			case SIZE_BOTTOM:
				m_selection->h = UTIL_MAX(round((e->button.y - m_origin.y) / ((float)m_scale_f) - m_selection->y), 4);
				break;
			case SIZE_LEFT:
				m_selection->x = UTIL_MAX(round((e->button.x - m_origin.x) / ((float)m_scale_f)), 0);
				m_selection->w = UTIL_MAX(m_selection_a.x - m_selection->x, 4);
				break;
			case SIZE_TOP:
				m_selection->y = UTIL_MAX(round((e->button.y - m_origin.y) / ((float)m_scale_f)), 0);
				m_selection->h = UTIL_MAX(m_selection_a.y - m_selection->y, 4);
				break;
			case SIZE_MOVE:
				m_selection->x = UTIL_MAX(round((e->button.x - m_selection_a.x) / ((float)m_scale_f)), 0);
				m_selection->y = UTIL_MAX(round((e->button.y - m_selection_a.y) / ((float)m_scale_f)), 0);
				break;
			default:;
			}
		} else {
			mouse_state(e);
		}
	} else if (e->type == SDL_MOUSEWHEEL && m_mouse_over) {
		auto mouse = *m_helper->util_mouse_pos();
		mouse.x -= get_origin_left();
		mouse.y -= get_origin_top();

		//        SDL_Point old_scale = {mouse.x * m_scale_f + m_origin.x, mouse.y * m_scale_f + m_origin.y};

		if (e->wheel.y > 0) /* TRIGGER_UP */
		{
			m_scale_f = UTIL_MIN(++m_scale_f, 8);
		} else {
			m_scale_f = UTIL_MAX(--m_scale_f, 1);
		}
		//mouse = { e->button.x, e->button.y };
		// translate(mouse.x, mouse.y);
		//        SDL_Point new_scale = {mouse.x * m_scale_f + m_origin.x, mouse.y * m_scale_f + m_origin.y};

		/* Move origin so the original center stays centered */

		//m_origin.x += (new_scale.x - old_scale.x);
		//m_origin.y += (new_scale.y - old_scale.y);

		m_origin.x = UTIL_MIN(m_origin.x, get_origin_left());
		m_origin.y = UTIL_MIN(m_origin.y, get_origin_top());

		m_selecting = false;
		m_sizing = false;
		was_handled = true;
	} else if (e->type == SDL_MOUSEBUTTONUP) {
		m_dragging = false;
		m_sizing = false;
		m_selecting = false;
	} else if (e->type == SDL_KEYDOWN && m_selection) {
		auto x = m_selection->x;
		auto y = m_selection->y;

		if (util_move_element(&x, &y, e->key.keysym.sym)) {
			m_selection->x = x;
			m_selection->y = y;
		}
	}
	return was_handled;
}

void coordinate_system::draw_foreground() const
{
	const auto step = 10 * m_scale_f;

	int x, y;
	int start_x, start_y;

	/* Draw custom grid, which adjusts to button dimensions */
	if (m_has_custom_grid) {
		auto step_x = m_grid_spacing.x * m_scale_f;
		auto step_y = m_grid_spacing.y * m_scale_f;
		start_x = get_origin_left() + ((m_origin.x - get_origin_left()) % step_x) + step_x;
		start_y = get_origin_top() + ((m_origin.y - get_origin_top()) % step_y) + step_y;

		for (x = start_x; x < get_right(); x += step_x) {
			m_helper->util_draw_line(x, get_origin_top(), x, get_bottom(), m_helper->get_palette()->gray());
		}

		for (y = start_y; y < get_bottom(); y += step_y) {
			m_helper->util_draw_line(get_origin_left(), y, get_right(), y, m_helper->get_palette()->gray());
		}

		if (m_has_rulers) {
			step_x = (m_grid_spacing.x + m_ruler_offset.x) * m_scale_f;
			step_y = (m_grid_spacing.y + m_ruler_offset.y) * m_scale_f;
			start_x = get_origin_left() + ((m_origin.x - get_origin_left()) % step_x) + step_x;
			start_y = get_origin_top() + ((m_origin.y - get_origin_top()) % step_y) + step_y;

			for (x = start_x; x < get_right(); x += step_x) {
				m_helper->util_draw_line(x, get_origin_top(), x, get_bottom(), m_helper->get_palette()->orange());
			}

			for (y = start_y; y < get_bottom(); y += step_y) {
				m_helper->util_draw_line(get_origin_left(), y, get_right(), y, m_helper->get_palette()->orange());
			}
		}
	}

	/* Draw Scale*/
	start_x = get_origin_left() + ((m_origin.x - get_origin_left()) % step) + step;
	start_y = get_origin_top() + ((m_origin.y - get_origin_top()) % step) + step;

	/* X Axis*/
	for (x = start_x; x < get_right(); x += step) {
		const auto flag = (x - m_origin.x) % 100 == 0 && (x - m_origin.x) != 0;
		auto flag2 = m_grid_spacing.x > 0 && (x - m_origin.x) % m_grid_spacing.x == 0;

		if (flag) {
			auto tag = std::to_string(((x - m_origin.x) / m_scale_f));
			const auto dim = m_helper->util_text_dim(&tag);
			m_helper->util_text_rot(&tag, UTIL_CLAMP(get_origin_left() + dim.h + 2, x + dim.h / 2, get_right() - 2),
									get_origin_top() - dim.w - 6, m_helper->get_palette()->white(), 90);
			m_helper->util_draw_line(x, get_origin_top() - 4, x, get_origin_top() + 4,
									 m_helper->get_palette()->white());
			if (!m_has_custom_grid)
				m_helper->util_draw_line(x, get_origin_top() + 4, x, get_bottom(), m_helper->get_palette()->gray());
		} else {
			m_helper->util_draw_line(x, get_origin_top() - 2, x, get_origin_top() + 2,
									 m_helper->get_palette()->white());
		}
	}

	/* Y Axis */
	for (y = start_y; y < get_bottom(); y += step) {
		const auto flag = (y - m_origin.y) % 100 == 0 && (y - m_origin.y) != 0;

		if (flag) {
			auto tag = std::to_string(((y - m_origin.y) / m_scale_f));
			const auto dim = m_helper->util_text_dim(&tag);

			m_helper->util_text(&tag, get_origin_left() - dim.w - 5,
								UTIL_CLAMP(get_origin_top() + 2, y - dim.h / 2, get_bottom() - dim.h - 2),
								m_helper->get_palette()->white());
			m_helper->util_draw_line(get_origin_left() - 4, y, get_origin_left() + 4, y,
									 m_helper->get_palette()->white());
			if (!m_has_custom_grid)
				m_helper->util_draw_line(get_origin_left() + 4, y, get_right(), y, m_helper->get_palette()->gray());
		} else {
			m_helper->util_draw_line(get_origin_left() - 2, y, get_origin_left() + 2, y,
									 m_helper->get_palette()->white());
		}
	}

	/* Draw origin cross (0/0) */
	m_helper->util_draw_line(m_dimensions.x, get_origin_top(), get_right() - 1, get_origin_top(),
							 m_helper->get_palette()->white());
	m_helper->util_draw_line(get_origin_left(), m_dimensions.y, get_origin_left(), get_bottom() - 1,
							 m_helper->get_palette()->white());

	/* Axe titles */
	auto t = m_helper->loc(LANG_LABEL_X_AXIS);
	auto dim = m_helper->util_text_dim(&t);
	m_helper->util_text(&t, get_origin_left() - dim.w - 15, get_origin_top() - dim.h, m_helper->get_palette()->white());

	t = m_helper->loc(LANG_LABEL_Y_AXIS);
	dim = m_helper->util_text_dim(&t);
	m_helper->util_text_rot(&t, get_origin_left(), get_origin_top() - 15 - dim.w, m_helper->get_palette()->white(), 90);

	t = sdl_helper::format(m_helper->loc(LANG_LABEL_SCALE).c_str(), m_scale_f);
	dim = m_helper->util_text_dim(&t);
	m_helper->util_text(&t, get_right() - dim.w - 5, m_dimensions.y + dim.h + 5, m_helper->get_palette()->white());

	/* Cross hair */
	SDL_Point mouse;
	SDL_GetMouseState(&mouse.x, &mouse.y);

	if (m_crosshair && m_size_mode == SIZE_NONE && sdl_helper::util_is_in_rect(&m_system_area, mouse.x, mouse.y)) {
		begin_draw();
		{
			translate(mouse.x, mouse.y);

			m_helper->util_draw_line(mouse.x, 0, mouse.x, m_system_area.h, m_helper->get_palette()->white());
			m_helper->util_draw_line(0, mouse.y, m_system_area.w, mouse.y, m_helper->get_palette()->white());
		}
		end_draw();
	} else if (m_size_mode != SIZE_NONE) {
		draw_rulers();
	}

	/* Border around the entire thing */
	if (m_border)
		m_helper->util_draw_rect(&m_dimensions, m_helper->get_palette()->white());
}

void coordinate_system::draw_background() const
{
	m_helper->util_fill_rect(&m_dimensions, m_helper->get_palette()->dark_gray());
}

void coordinate_system::set_dimensions(const SDL_Rect r)
{
	m_dimensions = r;
	m_system_area = {get_origin_left(), get_origin_top(), m_dimensions.w - m_origin_anchor.x,
					 m_dimensions.h - m_origin_anchor.y};
}

void coordinate_system::set_pos(const int x, const int y)
{
	m_origin.x -= m_dimensions.x - x;
	m_origin.y -= m_dimensions.y - y;
	m_dimensions.x = x;
	m_dimensions.y = y;
	m_system_area = {get_origin_left(), get_origin_top(), m_dimensions.w - m_origin_anchor.x,
					 m_dimensions.h - m_origin_anchor.y};
}

int coordinate_system::adjust(const int i) const
{
	return ceil(i / m_scale_f) * m_scale_f;
}

void coordinate_system::translate(int &x, int &y) const
{
	x -= m_dimensions.x + m_origin_anchor.x;
	y -= m_dimensions.y + m_origin_anchor.y;
}

void coordinate_system::draw_selection() const
{
	/* Draw selection separate, since it needs to be topmost */
	if (!SDL_RectEmpty(m_selection)) {
		auto temp = *m_selection;
		temp.x = temp.x * m_scale_f + get_origin_x();
		temp.y = temp.y * m_scale_f + get_origin_y();
		temp.w = temp.w * m_scale_f;
		temp.h = temp.h * m_scale_f;

		begin_draw();
		{
			m_helper->util_draw_rect(&temp, m_helper->get_palette()->red());
		}
		end_draw();
	}
}

void coordinate_system::draw_rulers() const
{
	if (!SDL_RectEmpty(m_selection)) {
		auto temp = *m_selection;
		begin_draw();
		{
			temp.x = temp.x * m_scale_f + get_origin_x();
			temp.y = temp.y * m_scale_f + get_origin_y();
			temp.w = temp.w * m_scale_f - 1;
			temp.h = temp.h * m_scale_f - 1;

			m_helper->util_draw_line(0, temp.y, m_system_area.w, temp.y, m_helper->get_palette()->white());
			m_helper->util_draw_line(0, temp.y + temp.h, m_system_area.w - 1, temp.y + temp.h,
									 m_helper->get_palette()->white());

			m_helper->util_draw_line(temp.x, 0, temp.x, m_system_area.h, m_helper->get_palette()->white());
			m_helper->util_draw_line(temp.x + temp.w, 0, temp.x + temp.w, m_system_area.h,
									 m_helper->get_palette()->white());
		}
		end_draw();
	}
}

/*
  Analyzes how the mouse is positioned
  to the selection box.
  Sets horizontal cursor id, if the mouse
  is close enough to either side of the selection.
  Same for up and down.
*/
#define EXTENDED_BORDER 8

void coordinate_system::mouse_state(SDL_Event *event)
{
	if (!m_selection)
		return;

	const SDL_Point mouse = {event->button.x, event->button.y};
	SDL_Rect selection = {m_selection->x * m_scale_f + m_origin.x, m_selection->y * m_scale_f + m_origin.y,
						  m_selection->w * m_scale_f, m_selection->h * m_scale_f};

	if (SDL_RectEmpty(&selection))
		return;

	if (in_range(mouse.x, selection.x, EXTENDED_BORDER) &&
		in_between(mouse.y, m_selection->y, m_selection->y + m_selection->h, m_origin.y)) {
		m_size_mode = SIZE_LEFT;
		m_helper->set_cursor(CURSOR_SIZE_H);
	} else if (in_range(mouse.x, selection.w + selection.x, EXTENDED_BORDER) &&
			   in_between(mouse.y, m_selection->y, m_selection->y + m_selection->h, m_origin.y)) {
		m_size_mode = SIZE_RIGHT;
		m_helper->set_cursor(CURSOR_SIZE_H);
	} else if (in_range(mouse.y, selection.y, EXTENDED_BORDER) &&
			   in_between(mouse.x, m_selection->x, m_selection->x + m_selection->w, m_origin.x)) {
		m_size_mode = SIZE_TOP;
		m_helper->set_cursor(CURSOR_SIZE_V);
	} else if (in_range(mouse.y, selection.h + selection.y, EXTENDED_BORDER) &&
			   in_between(mouse.x, m_selection->x, m_selection->x + m_selection->w, m_origin.x)) {
		m_size_mode = SIZE_BOTTOM;
		m_helper->set_cursor(CURSOR_SIZE_V);
	} else if (sdl_helper::util_is_in_rect(&selection, event->button.x, event->button.y)) {
		m_size_mode = SIZE_MOVE;
		m_helper->set_cursor(CURSOR_SIZE_ALL);
	} else if (sdl_helper::util_is_in_rect(&m_system_area, event->button.x, event->button.y))
	/*
            Only reset the cursor within the coordinate system,
            because outside it might be set to I_BEAM by a text box
        */
	{
		m_size_mode = SIZE_NONE;
		m_helper->set_cursor(CURSOR_ARROW);
	}
}
