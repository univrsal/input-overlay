#include "DialogNewElement.hpp"

void DialogNewElement::close(void)
{
	Dialog::close();
	if (m_cs)
		delete m_cs;
	m_cs = nullptr;
}

void DialogNewElement::init()
{
	Dialog::init();
	SDL_Rect temp = { get_left() + 8, get_top() + 30, m_dimensions.w - 120, m_dimensions.h - 46 };
	m_cs = new CoordinateSystem(SDL_Point { 90, 90 }, temp, m_helper);
	m_cs->enable_border();
	set_flags(DIALOG_CENTERED | DIALOG_TOP_MOST);
}

void DialogNewElement::action_performed(int8_t action_id)
{
	
}

void DialogNewElement::draw_background(void)
{
	Dialog::draw_background();
	m_cs->draw_background();
}

void DialogNewElement::draw_foreground(void)
{
	Dialog::draw_foreground();
	m_cs->draw_foreground();
}

bool DialogNewElement::handle_events(SDL_Event * event)
{
	bool was_handled = false;
	if (Dialog::handle_events(event))
		was_handled = true;

	if (event->type == SDL_WINDOWEVENT)
	{
		if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		{
			m_cs->set_pos(get_left() + 8, get_top() + 30);
		}
	}

	if (m_cs->handle_events(event))
		was_handled = true;
	return was_handled;
}
