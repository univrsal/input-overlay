#include "AtlasSelector.hpp"

AtlasSelector::AtlasSelector(int8_t id, int x, int y, int w, int h, Texture * t, Dialog * parent)
{
	SDL_Rect temp = { x, y, w, h };
	init(parent, temp, id);
	m_atlas = t;
}

AtlasSelector::~AtlasSelector()
{
	close();
}

void AtlasSelector::close(void)
{
	GuiElement::close();
	if (m_cs)
		delete m_cs;
	m_cs = nullptr;

}

void AtlasSelector::init(Dialog *parent, SDL_Rect dim, int8_t id)
{
	GuiElement::init(parent, dim, id);
	m_cs = new CoordinateSystem(SDL_Point{ 90, 90 }, m_dimensions, get_helper());
	m_cs->enable_border();
}

void AtlasSelector::draw_foreground(void)
{
	m_cs->draw_foreground();

	if (!SDL_RectEmpty(&m_selection))
	{
		SDL_Rect temp = m_selection;
		m_cs->crop_rect(SDL_Rect{}, temp);
		get_helper()->util_draw_rect(&temp, get_helper()->palette()->red());
	}
}

void AtlasSelector::draw_background(void)
{
	m_cs->draw_background();
	SDL_Rect temp = m_atlas->get_dim();
	SDL_Rect temp_mapping = m_atlas->get_dim();

	temp.x = m_cs->get_origin()->x;
	temp.y = m_cs->get_origin()->y;
	temp.w *= m_cs->get_scale();
	temp.h *= m_cs->get_scale();


	m_cs->crop_rect(temp_mapping, temp);

	m_atlas->draw(get_helper()->renderer(), &temp, &temp_mapping);
}

bool AtlasSelector::handle_events(SDL_Event * event)
{
	bool was_handled = false;

	if (m_cs->handle_events(event))
		was_handled = true;

	if (event->type == SDL_MOUSEBUTTONDOWN)
	{
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			if (get_helper()->util_is_in_rect(m_cs->get_system_area(),
				event->button.x, event->button.y))
			{
				if (SDL_RectEmpty(&m_selection))
				{
					m_selection_a.x = event->button.x;
					m_selection_a.y = event->button.y;
					m_selecting = true;
					was_handled = true;
				}
				else
				{
					if (m_size_mode != SIZE_NONE)
					{
						m_selection_a.x = m_selection.x + m_selection.w;
						m_selection_a.y = m_selection.y + m_selection.h;
						m_sizing = true;
					}
				}
			}
		}
	}
	else if (event->type == SDL_MOUSEMOTION)
	{
		if (m_selecting)
		{
			m_selection.x = m_cs->adjust(UTIL_MIN(m_selection_a.x, event->button.x));
			m_selection.y = m_cs->adjust(UTIL_MIN(m_selection_a.y, event->button.y));
			m_selection.w = m_cs->adjust(SDL_abs(m_selection_a.x - event->button.x));
			m_selection.h = m_cs->adjust(SDL_abs(m_selection_a.y - event->button.y));
		}
		else
		{
			if (!m_sizing)
			{
				mouse_state(event);
			}
			else
			{
				switch (m_size_mode)
				{
				case SIZE_RIGHT:
					m_selection.w = event->button.x - m_selection.x;
					break;
				case SIZE_LEFT:
					m_selection.x = event->button.x;
					m_selection.w = m_selection_a.x - m_selection.x;
					break;
				case SIZE_BOTTOM:
					m_selection.h = event->button.y - m_selection.y;
					break;
				case SIZE_TOP:
					m_selection.y = event->button.y;
					m_selection.h = m_selection_a.y - m_selection.y;
					break;
				}
			}
		}
	}
	else if (event->type == SDL_MOUSEBUTTONUP)
	{
		m_selecting = false;
		m_sizing = false;
		mouse_state(event);
	}

	return was_handled;
}

SDL_Rect * AtlasSelector::get_selection()
{
	return &m_selection;
}

uint8_t AtlasSelector::get_cursor(void)
{
	return CURSOR_SPECIAL;
}

void AtlasSelector::resize()
{
	m_cs->set_pos(m_dimensions.x, m_dimensions.y);
	m_cs->set_dimensions(m_dimensions);
}

inline bool AtlasSelector::in_range(int a, int b, int range)
{
	return a <= b + range && a >= b - range;
}

/*
	Analysises how the mouse is positioned
	to the selection box.
	Returns horizontal cursor id, if the mouse
	is close enough to either side of the selection.
	Same for up and down.
*/
#define EXTENDED_BORDER 2
void AtlasSelector::mouse_state(SDL_Event * event)
{
	SDL_Point mouse = { event->button.x, event->button.y };
	if (in_range(mouse.x, m_selection.x, EXTENDED_BORDER))
	{
		m_size_mode = SIZE_LEFT;
		get_helper()->set_cursor(CURSOR_SIZE_H);
	}
	else if (in_range(mouse.x, m_selection.x + m_selection.w, EXTENDED_BORDER))
	{
		m_size_mode = SIZE_RIGHT;
		get_helper()->set_cursor(CURSOR_SIZE_H);
	}
	else if (in_range(mouse.y, m_selection.y, EXTENDED_BORDER))
	{
		m_size_mode = SIZE_TOP;
		get_helper()->set_cursor(CURSOR_SIZE_V);
	}
	else if (in_range(mouse.y, m_selection.y + m_selection.h, EXTENDED_BORDER))
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
