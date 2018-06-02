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
	SDL_Rect temp = m_selection;
	m_cs->crop_rect(SDL_Rect {}, temp);
	get_helper()->util_draw_rect(&temp, get_helper()->palette()->red());
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
			if (SDL_RectEmpty(&m_selection))
			{
				m_selection_a.x = event->button.x;
				m_selection_a.y = event->button.y;
				m_selecting = true;
				was_handled = true;
			}
			else
			{

			}
		}
	}
	else if (event->type == SDL_MOUSEMOTION)
	{
		if (m_selecting)
		{
			m_selection.x = m_cs->adjust(SDL_min(m_selection_a.x, event->button.x));
			m_selection.y = m_cs->adjust(SDL_min(m_selection_a.y, event->button.y));
			m_selection.w = m_cs->adjust(SDL_abs(m_selection_a.x - event->button.x));
			m_selection.h = m_cs->adjust(SDL_abs(m_selection_a.y - event->button.y));
		}
	}
	else if (event->type == SDL_MOUSEBUTTONUP)
	{
		m_selecting = false;
	}

	return was_handled;
}

SDL_Rect * AtlasSelector::get_selection()
{
	return &m_selection;
}
