#include "CoordinateSystem.hpp"
bool CoordinateSystem::handle_events(SDL_Event * e)
{
	bool was_handled = false;

	if (e->type == SDL_MOUSEBUTTONDOWN)
	{
		if (e->button.button == SDL_BUTTON_RIGHT)
		{
			if (m_helper->util_is_in_rect(&m_system_area, e->button.x, e->button.y))
			{
				m_drag_offset = { e->button.x - m_origin.x,
					e->button.y - m_origin.y };
				m_dragging = true;
				was_handled = true;
			}
		}
		else if (e->button.button == SDL_BUTTON_LEFT)
		{
			if (get_helper()->util_is_in_rect(get_system_area(),
				e->button.x, e->button.y))
			{
				if (m_selection)
				{
					if (SDL_RectEmpty(m_selection))
					{
						m_selection_a.x = e->button.x;
						m_selection_a.y = e->button.y;
						m_selecting = true;
						was_handled = true;
					}
					else
					{
						if (m_size_mode != SIZE_NONE)
						{
							m_sizing = true;
							m_selection_a = { m_selection->x + m_selection->w,
								m_selection->y + m_selection->h };
						}
					}
				}
			}
		}
	}
	else if (e->type == SDL_MOUSEMOTION)
	{
		if (m_dragging && (e->motion.state & SDL_BUTTON_RMASK))
		{
			m_origin.x = SDL_min(e->button.x - m_drag_offset.x, get_origin_left());
			m_origin.y = SDL_min(e->button.y - m_drag_offset.y, get_origin_top());
			was_handled = true;
		}
		else if (m_selecting && (e->motion.state & SDL_BUTTON_LMASK))
		{
			m_selection->x = ceil((UTIL_MIN(m_selection_a.x, e->button.x) - m_origin.x) / m_scale_f);
			m_selection->y = ceil((UTIL_MIN(m_selection_a.y, e->button.y) - m_origin.y) / m_scale_f);
			m_selection->w = ceil(SDL_abs(m_selection_a.x - e->button.x) / m_scale_f);
			m_selection->h = ceil(SDL_abs(m_selection_a.y - e->button.y) / m_scale_f);
		}
		else if (m_sizing && (e->motion.state & SDL_BUTTON_LMASK))
		{

			switch (m_size_mode)
			{
				case SIZE_RIGHT:
					m_selection->w = UTIL_MAX(ceil((e->button.x - m_origin.x) / m_scale_f - m_selection->x), 4);
					break;
				case SIZE_BOTTOM:
					m_selection->h = UTIL_MAX(ceil((e->button.y - m_origin.y) / m_scale_f - m_selection->y), 4);
					break;
				case SIZE_LEFT:
					m_selection->x = UTIL_MAX(ceil((e->button.x - m_origin.x) / m_scale_f), 0);
					m_selection->w = UTIL_MAX(m_selection_a.x - m_selection->x, 4);
					break;
				case SIZE_TOP:
					m_selection->y = UTIL_MAX(ceil((e->button.y - m_origin.y) / m_scale_f), 0);
					m_selection->h = UTIL_MAX(m_selection_a.y - m_selection->y, 4);
					break;
			}
		}
		else
		{
			mouse_state(e);
		}
	}
	else if (e->type == SDL_MOUSEWHEEL)
	{
		if (e->wheel.y > 0) /* UP */
		{
			m_scale_f = SDL_min(m_scale_f++, 8);
		}
		else
		{
			m_scale_f = SDL_max(m_scale_f--, 1);
		}

		m_origin.x = SDL_min(m_origin.x, get_origin_left());
		m_origin.y = SDL_min(m_origin.y, get_origin_top());
		was_handled = true;

		m_selecting = false;
		m_sizing = false;
		was_handled = true;
	}
	else if (e->type == SDL_MOUSEBUTTONUP)
	{
		m_dragging = false;
		m_sizing = false;
		m_selecting = false;
	}

	return was_handled;
}

void CoordinateSystem::draw_foreground(void)
{
	/* Draw Scale*/
	int step = 10 * m_scale_f;
	int start;

	/* I think this makes sense */

	start = get_origin_left() + ((m_origin.x - get_origin_left()) % step) + step;
	
	/* X Axis*/
	for (int x = start; x < get_right(); x += step)
	{
		bool flag = (x - m_origin.x) % 100 == 0 && (x - m_origin.x) != 0;
		if (flag)
		{
			std::string tag = std::to_string((int) ((x - m_origin.x) / m_scale_f));
			SDL_Rect dim = m_helper->util_text_dim(&tag);
			m_helper->util_text(&tag,
				UTIL_CLAMP(get_origin_left() + dim.h + 2,x + dim.h / 2, get_right() - 2),
				get_origin_top() - dim.w - 6, m_helper->palette()->white(), 90);

			m_helper->util_draw_line(x, get_origin_top() - 4, x, get_origin_top() + 4, m_helper->palette()->white());
			m_helper->util_draw_line(x, get_origin_top() + 4, x, get_bottom(), m_helper->palette()->gray());
		}
		else
		{
			m_helper->util_draw_line(x, get_origin_top() - 2, x, get_origin_top() + 2, m_helper->palette()->white());
		}
	}
	
	start = get_origin_top() + ((m_origin.y - get_origin_top()) % ((int) (10 * m_scale_f))) + step;
	
	for (int y = start; y < get_bottom(); y += step)
	{
		bool flag = (y - m_origin.y) % 100 == 0 && (y - m_origin.y) != 0;

		if (flag)
		{
			std::string tag = std::to_string((int) ((y - m_origin.y) / m_scale_f));
			SDL_Rect dim = m_helper->util_text_dim(&tag);

			m_helper->util_text(&tag, get_origin_left() - dim.w - 5,
				UTIL_CLAMP(get_origin_top() + 2, y - dim.h / 2, get_bottom() - dim.h - 2),
				m_helper->palette()->white());
			m_helper->util_draw_line(get_origin_left() - 4, y, get_origin_left() + 4, y, m_helper->palette()->white());
			m_helper->util_draw_line(get_origin_left() + 4, y, get_right(), y, m_helper->palette()->gray());
		}
		else
		{
			m_helper->util_draw_line(get_origin_left() - 2, y, get_origin_left() + 2, y, m_helper->palette()->white());
		}
	}

	/* Draw origin cross (0/0) */
	m_helper->util_draw_line(m_dimensions.x, get_origin_top(),
		get_right() - 1, get_origin_top(), m_helper->palette()->white());
	m_helper->util_draw_line(get_origin_left(), m_dimensions.y,
		get_origin_left(), get_bottom() - 1, m_helper->palette()->white());

	/* Axe titles */
	std::string t = "X in pixels";
	SDL_Rect dim = m_helper->util_text_dim(&t);

	m_helper->util_text(&t, get_origin_left() - dim.w - 15, get_origin_top() - dim.h, m_helper->palette()->white());
	t = "Y in pixels";
	dim = m_helper->util_text_dim(&t);
	m_helper->util_text(&t, get_origin_left(), get_origin_top() - 15 - dim.w, m_helper->palette()->white(), 90);

	t = "Scale: " + std::to_string((int) m_scale_f);
	dim = m_helper->util_text_dim(&t);
	m_helper->util_text(&t, get_right() - dim.w - 5, m_dimensions.y + dim.h + 5, m_helper->palette()->white());

	/* Draw selection */
	if (!SDL_RectEmpty(m_selection))
	{
		SDL_Rect temp = *m_selection;
		temp.x = temp.x * m_scale_f + get_origin_x();
		temp.y = temp.y * m_scale_f + get_origin_y();
		temp.w = temp.w * m_scale_f;
		temp.h = temp.h * m_scale_f;
		
		begin_draw();
		{
			get_helper()->util_draw_rect(&temp, get_helper()->palette()->red());
		}
		end_draw();
	}

	/* Border around the entire thing */
	if (m_border)
		m_helper->util_draw_rect(&m_dimensions, m_helper->palette()->white());
}

void CoordinateSystem::draw_background(void)
{
	m_helper->util_fill_rect(&m_dimensions, m_helper->palette()->dark_gray());
}

void CoordinateSystem::set_dimensions(SDL_Rect r)
{
	m_dimensions = r;
	m_system_area = { get_origin_left(), get_origin_top(),
		m_dimensions.w - m_origin_anchor.x, m_dimensions.h - m_origin_anchor.y };
}

void CoordinateSystem::set_pos(int x, int y)
{
	m_origin.x -= m_dimensions.x - x;
	m_origin.y -= m_dimensions.y - y;
	m_dimensions.x = x; m_dimensions.y = y;
	m_system_area = { get_origin_left(), get_origin_top(),
		m_dimensions.w - m_origin_anchor.x, m_dimensions.h - m_origin_anchor.y };
}

int CoordinateSystem::adjust(int i)
{
	return ceil(i / m_scale_f) * m_scale_f;
}

/*
  Analyzes how the mouse is positioned
  to the selection box.
  Sets horizontal cursor id, if the mouse
  is close enough to either side of the selection.
  Same for up and down.
*/
#define EXTENDED_BORDER 2
void CoordinateSystem::mouse_state(SDL_Event * event)
{
	if (!m_selection)
		return;

	SDL_Point mouse = { event->button.x, event->button.y };
	if (in_range(mouse.x, m_selection->x * m_scale_f + m_origin.x, EXTENDED_BORDER)
		&& in_between(mouse.y, m_selection->y, m_selection->y + m_selection->h, m_origin.y))
	{
			m_size_mode = SIZE_LEFT;
			get_helper()->set_cursor(CURSOR_SIZE_H);
	}
	else if (in_range(mouse.x, (m_selection->x + m_selection->w) * m_scale_f + m_origin.x,
		EXTENDED_BORDER)
		&& in_between(mouse.y, m_selection->y, m_selection->y + m_selection->h, m_origin.y))
	{
		m_size_mode = SIZE_RIGHT;
		get_helper()->set_cursor(CURSOR_SIZE_H);
	}
	else if (in_range(mouse.y, m_selection->y * m_scale_f + m_origin.y, EXTENDED_BORDER)
		&& in_between(mouse.x, m_selection->x, m_selection->x + m_selection->w, m_origin.x))
	{
		m_size_mode = SIZE_TOP;
		get_helper()->set_cursor(CURSOR_SIZE_V);
	}
	else if (in_range(mouse.y, (m_selection->y + m_selection->h) * m_scale_f + m_origin.y,
		EXTENDED_BORDER)
		&& in_between(mouse.x, m_selection->x, m_selection->x + m_selection->w, m_origin.x))
	{
		m_size_mode = SIZE_BOTTOM;
		get_helper()->set_cursor(CURSOR_SIZE_V);
	}
	else
	{
		m_size_mode = SIZE_NONE;
		get_helper()->set_cursor(CURSOR_ARROW);
	}
}
