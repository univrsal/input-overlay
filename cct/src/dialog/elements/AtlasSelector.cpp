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
	SDL_Rect temp;
	SDL_Rect temp_mapping = m_atlas->get_dim();

	temp.x = m_cs->get_origin()->x;
	temp.y = m_cs->get_origin()->y;
	temp.w = temp_mapping.w * m_cs->get_scale();
	temp.h = temp_mapping.h * m_cs->get_scale();


	if (m_cs->crop_rect(temp_mapping, temp))
	{
		m_atlas->draw(get_helper()->renderer(), &temp, &temp_mapping);
		
		get_helper()->util_draw_rect(&temp, get_helper()->palette()->red());
	}
}

void AtlasSelector::draw_background(void)
{
	m_cs->draw_background();

}

bool AtlasSelector::handle_events(SDL_Event * event)
{
	bool was_handled = false;

	if (m_cs->handle_events(event))
		was_handled = true;

	return was_handled;
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
