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
	SDL_Rect temp = { get_left(), get_top() + 30, m_dimensions.w - 16, m_dimensions.h - 80 };
	m_cs = new CoordinateSystem(SDL_Point { 8, 40 }, temp, m_helper);
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
	if (m_cs->handle_events(event))
		was_handled = true;
	return was_handled;
}
