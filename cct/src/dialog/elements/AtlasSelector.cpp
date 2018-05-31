#include "AtlasSelector.hpp"

AtlasSelector::AtlasSelector(int8_t id, int x, int y, int w, int h, Texture * t, Dialog * parent)
{
	SDL_Rect temp = { x, y, w, h };
	init(parent, temp, id);
}

AtlasSelector::~AtlasSelector()
{
	close();
}

void AtlasSelector::close(void)
{
	GuiElement::close();
}

void AtlasSelector::draw_foreground(void)
{
}

void AtlasSelector::draw_background(void)
{
}

bool AtlasSelector::handle_events(SDL_Event * event)
{
	return false;
}

SDL_Rect * AtlasSelector::get_selection()
{
	return &m_selection;
}
