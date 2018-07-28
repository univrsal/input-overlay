#include "Checkbox.hpp"
/**
 * Created by univrsal on 25.06.20178.
 * This file is part of reloded which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/reloded
 */

Checkbox::Checkbox(int8_t id, int x, int y, const char * text, Dialog * parent)
{
	SDL_Rect temp = { x, y, 0, 0 };
	m_checkbox = { x, y, CHECKBOX_SIZE, CHECKBOX_SIZE };

	m_label = new Label(id, x + CHECKBOX_SIZE + 5, y, text, parent);

	if (m_label->get_height() > CHECKBOX_SIZE)
	{
		m_checkbox.y = m_label->get_height() / 2 - CHECKBOX_SIZE / 2;
		temp.h = m_label->get_height();
	}
	else
	{
		m_label->set_pos(x + CHECKBOX_SIZE + 5,
			y + CHECKBOX_SIZE / 2 - m_label->get_height() / 2);
		temp.h = CHECKBOX_SIZE;
	}

	temp.w = m_label->get_width() + CHECKBOX_SIZE + 5;
	init(parent, temp, id);
}

Checkbox::Checkbox(int8_t id, int x, int y, const char * text, Dialog * parent, bool state)
	: Checkbox(id, x, y, text, parent)
{
	m_state = state;
}

Checkbox::~Checkbox()
{
	close();
}

void Checkbox::close(void)
{
	if (m_label)
		delete m_label;
	m_label = nullptr;
}

bool Checkbox::can_select(void)
{
	return true;
}

void Checkbox::select_state(bool state)
{
	m_focused = state;
}

void Checkbox::draw_background(void)
{
	m_label->draw_background();

	get_helper()->util_fill_rect(get_left(), get_top()
		+ m_checkbox.y, CHECKBOX_SIZE, CHECKBOX_SIZE, get_helper()->palette()->dark_gray());
	get_helper()->util_draw_rect(get_left(), get_top()
		+ m_checkbox.y, CHECKBOX_SIZE, CHECKBOX_SIZE, get_helper()->palette()->light_gray());
	if (m_focused)
	{
		get_helper()->util_draw_rect(get_left() - 2, get_top() - 2,
			get_width() + 4, get_height() + 4, get_helper()->palette()->light_gray());
	}
}

void Checkbox::draw_foreground(void)
{
	m_label->draw_foreground();
	if (m_state)
	{
		get_helper()->util_fill_rect(get_left() + 4, get_top() + m_checkbox.y + 4,
			CHECKBOX_SIZE - 8, CHECKBOX_SIZE - 8, get_helper()->palette()->green());
	}
}

bool Checkbox::handle_events(SDL_Event * event)
{
	bool handled = false;
	if (event->type == SDL_MOUSEBUTTONDOWN)
	{
		if (event->button.button == SDL_BUTTON_LEFT && is_mouse_over(event->button.x, event->button.y))
		{
			handled = true;
			m_state = !m_state;
			get_parent()->action_performed(get_id());
		}
	}
	else if (m_focused && event->type == SDL_KEYUP)
	{
		if (event->key.keysym.sym == SDLK_RETURN)
		{
			m_state = !m_state;
			get_parent()->action_performed(get_id());
		}
	}
	return handled;
}
