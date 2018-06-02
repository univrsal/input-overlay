#include "CoordinateSystem.hpp"

/*
	This crops any rectangle to fit
	inside the coordinate boundaries
	it'll also crop the texture coutout
	accordingly.
	returns true, if the cropping succeeded
	returns false, if the rectangle is out of
	bounds or has no size
*/
bool CoordinateSystem::crop_rect(SDL_Rect & mapping, SDL_Rect& destination)
{
	SDL_Rect intersect;
	
	if (SDL_IntersectRect(&destination, &m_system_area, &intersect))
	{
		if (intersect.w == destination.w && intersect.h == destination.h)
			return true; /* No cropping needed */

		if (!SDL_RectEmpty(&mapping))
		{
			mapping.x += (intersect.x - destination.x) / m_scale_f;
			mapping.y += (intersect.y - destination.y) / m_scale_f;
			mapping.w = intersect.w / m_scale_f;
			mapping.h = intersect.h / m_scale_f;
		}
		
		destination = intersect;
		return true;
	}
	else
	{
		return false;
	}
}

bool CoordinateSystem::handle_events(SDL_Event * e)
{
	bool was_handled = false;

	if (e->type == SDL_MOUSEBUTTONDOWN)
	{
		if (e->button.button == SDL_BUTTON_RIGHT)
		{
			if (m_helper->util_is_in_rect(&m_system_area, e->button.x, e->button.y))
			{
				m_drag_offset = { (e->button.x - m_origin.x) / m_scale_f * m_scale_f,
					(e->button.y - m_origin.y) / m_scale_f * m_scale_f };
				m_dragging = true;
				was_handled = true;
			}
		}
	}
	else if (e->type == SDL_MOUSEMOTION)
	{
		if (m_dragging && (e->motion.state & SDL_BUTTON_RMASK))
		{
			/* Align movement to grid*/
			m_origin.x = (SDL_min(e->button.x - m_drag_offset.x, get_origin_left()))
				/ m_scale_f * m_scale_f;
			m_origin.y = (SDL_min(e->button.y - m_drag_offset.y, get_origin_top()))
				/ m_scale_f * m_scale_f;
			was_handled = true;
		}
	}
	else if (e->type == SDL_MOUSEWHEEL)
	{
		if (e->wheel.y > 0) /* UP */
		{
			m_scale_f = SDL_min(m_scale_f++, 7);
		}
		else
		{
			m_scale_f = SDL_max(m_scale_f--, 1);
		}
		was_handled = true;
	}
	else if (e->type == SDL_MOUSEBUTTONUP)
	{
		m_dragging = false;
	}

	return was_handled;
}

void CoordinateSystem::draw_foreground(void)
{
	/* Draw Scale*/
	int step = 10 * m_scale_f;
	int start;

	/* I think this makes sense */
	start = get_origin_left() + ((m_origin.x - get_origin_left()) % (10 * m_scale_f)) + step;
	
	/* X Axis*/
	for (int x = start; x < get_right(); x += step)
	{
		bool flag = (x - m_origin.x) % 100 == 0 && (x - m_origin.x) != 0;
		if (flag)
		{
			std::string tag = std::to_string((x - m_origin.x) / m_scale_f);
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
	
	start = get_origin_top() + ((m_origin.y - get_origin_top()) % (10 * m_scale_f)) + step;

	for (int y = start; y < get_bottom(); y += step)
	{
		bool flag = (y - m_origin.y) % 100 == 0 && (y - m_origin.y) != 0;

		if (flag)
		{
			std::string tag = std::to_string((y - m_origin.y) / m_scale_f);
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

	t = "Scale: " + std::to_string(m_scale_f);
	dim = m_helper->util_text_dim(&t);
	m_helper->util_text(&t, get_right() - dim.w - 5, m_dimensions.y + dim.h + 5, m_helper->palette()->white());

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
	//SDL_Point diff = {, m_dimensions.y - y };
	m_origin.x -= m_dimensions.x - x;
	m_origin.y -= m_dimensions.y - y;
	m_dimensions.x = x; m_dimensions.y = y;
	m_system_area = { get_origin_left(), get_origin_top(),
		m_dimensions.w - m_origin_anchor.x, m_dimensions.h - m_origin_anchor.y };

}

int CoordinateSystem::adjust(int i)
{
	return i / m_scale_f * m_scale_f;
}
