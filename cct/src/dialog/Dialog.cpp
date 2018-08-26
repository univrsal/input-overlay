#include "Dialog.hpp"
/**
 * Created by univrsal on 23.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

Dialog::Dialog(SDL_Helper * sdl, SDL_Rect size, std::string title)
{
	m_helper = sdl;
	m_dimensions = size;
	m_title = title;
	m_title_bar = { m_dimensions.x + 5, m_dimensions.y + 5, m_dimensions.w - 10, 20 };
}

Dialog::Dialog(SDL_Helper * sdl, SDL_Point size, std::string title)
{
	SDL_Point * window_size = sdl->util_window_size();
	SDL_Rect temp = { (*window_size).x / 2 - size.x / 2, (*window_size).y / 2 - size.y / 2, size.x, size.y };
	m_helper = sdl;
	m_dimensions = temp;
	m_title = title;
	m_title_bar = { m_dimensions.x + 5, m_dimensions.y + 5, m_dimensions.w - 10, 20 };
}

Dialog::~Dialog()
{
	close();
}

void Dialog::init()
{
	add(new Label(0, 8, 6, m_title.c_str(), this));

	SDL_Point * temp = m_helper->util_window_size();

	if (m_flags & DIALOG_FLUID)
	{
		m_dimensions.w = temp->x - FLUID_BORDER;
		m_dimensions.h = temp->y - FLUID_BORDER;
		m_dimensions.x = temp->x / 2 - m_dimensions.w / 2;
		m_dimensions.y = temp->y / 2 - m_dimensions.h / 2;
	}

	if (m_flags & DIALOG_CENTERED)
	{
		m_dimensions.x = temp->x / 2 - m_dimensions.w / 2;
		m_dimensions.y = temp->y / 2 - m_dimensions.h / 2;
	}
	m_title_bar = { m_dimensions.x + 5, m_dimensions.y + 5, m_dimensions.w - 10, 20 };
}

void Dialog::draw_background(void)
{
	if (m_flags & DIALOG_TOP_MOST)
	/* Tint background in gray */
	{
		SDL_Point * s = m_helper->util_window_size();
		SDL_Rect temp = { 0, 0, s->x, s->y };
		m_helper->util_fill_rect(&temp, m_helper->palette()->dark_gray(), 200);
	}

	// Dialog box
	m_helper->util_fill_rect_shadow(&m_dimensions, m_helper->palette()->get_accent());
	m_helper->util_draw_rect(&m_dimensions, m_helper->palette()->dark_gray());

	// Dialog title bar
	m_helper->util_fill_rect(&m_title_bar, m_helper->palette()->light_gray());

	for (auto const &element : m_screen_elements)
	{
		element->draw_background();
	}
}

void Dialog::draw_foreground(void)
{
	for (auto const &element : m_screen_elements)
	{
		element->draw_foreground();
	}
}

void Dialog::close(void)
{
	m_tab_items.clear();
	m_screen_elements.clear();
}

bool Dialog::handle_events(SDL_Event * event)
{
	bool was_handled = false;

	if (m_flags & DIALOG_DRAGGABLE)
	{
		if (event->type == SDL_MOUSEBUTTONDOWN)
		{
			if (event->button.button == SDL_BUTTON_LEFT)
			{
				if (m_helper->util_is_in_rect(&m_title_bar, event->button.x, event->button.y))
				{
					m_is_dragging = true;
					m_offset_x = event->button.x - m_title_bar.x;
					m_offset_y = event->button.y - m_title_bar.y;
					was_handled = true;
				}

				if (m_helper->util_is_in_rect(&m_dimensions, event->button.x, event->button.y))
				{
					action_performed(ACTION_FOCUSED);
					was_handled = true;
				}
			}
		}
		else if (event->type == SDL_MOUSEBUTTONUP)
		{
			if (event->button.button == SDL_BUTTON_LEFT)
			{
				m_is_dragging = false;
			}
		}
		else if (event->type == SDL_MOUSEMOTION)
		{
			if (m_is_dragging)
			{
				m_dimensions.x = event->button.x - m_offset_x;
				m_dimensions.y = event->button.y - m_offset_y;
				m_title_bar = { m_dimensions.x + 5, m_dimensions.y + 5, m_dimensions.w - 10, 20 };
				was_handled = true;
			}
		}
		else if (event->type == SDL_KEYDOWN)
		{
			if (event->key.keysym.sym == SDLK_RETURN)
			{
				action_performed(ACTION_OK);
				was_handled = true;
			}
		}
	}

	if (event->type == SDL_WINDOWEVENT)
	{
		if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		{
			/*
				Assure that dialogs aren't outside of
				the main window after resizing
			*/

			SDL_Point * temp = m_helper->util_window_size();

			if (temp->x < get_right())
			{
				m_dimensions.x = temp->x - m_dimensions.w;
			}

			if (temp->y < get_bottom())
			{
				m_dimensions.y = temp->y - m_dimensions.h;
			}

			if (m_flags & DIALOG_FLUID)
			{
				m_dimensions.w = temp->x - FLUID_BORDER;
				m_dimensions.h = temp->y - FLUID_BORDER;
				m_dimensions.x = temp->x / 2 - m_dimensions.w / 2;
				m_dimensions.y = temp->y / 2 - m_dimensions.h / 2;
			}

			if (m_flags & DIALOG_CENTERED)
			{
				m_dimensions.x = temp->x / 2 - m_dimensions.w / 2;
				m_dimensions.y = temp->y / 2 - m_dimensions.h / 2;
			}

			m_title_bar = { m_dimensions.x + 5, m_dimensions.y + 5, m_dimensions.w - 10, 20 };
		}
	}
	else if (event->type == SDL_KEYDOWN)
	{
		if (event->key.keysym.sym == SDLK_TAB)
		{
			for (auto const& e : m_tab_items)
			{
				e->select_state(false);
			}

			m_selected_element += m_helper->is_shift_down() ? -1 : 1;

			if (m_selected_element < 0)
				m_selected_element = (int16_t) m_tab_items.size() - 1;
			else if (m_selected_element > (int16_t) m_tab_items.size() - 1)
				m_selected_element = 0;

			m_tab_items[m_selected_element]->select_state(true);
		}
	}

	bool cursor_handled = false;
	bool element_handled = false;

	for (auto const& element : m_screen_elements)
	{
		if (element->handle_events(event, element_handled))
		{
			element_handled = true;
		}

		if (!cursor_handled && m_helper->util_is_in_rect(element->get_dimensions(), event->button.x, event->button.y))
		{
			cursor_handled = true;
			m_helper->set_cursor(element->get_cursor());
		}
	}

	if (!cursor_handled)
		m_helper->set_cursor(CURSOR_ARROW);
	return was_handled || element_handled;
}

void Dialog::action_performed(int8_t action_id)
{
	switch (action_id)
	{
	case ACTION_FOCUSED:
		if (m_flags & DIALOG_TEXTINPUT)
		{
			SDL_StartTextInput();
		}
		break;
	case ACTION_UNFOCUSED:
		if (m_flags & DIALOG_TEXTINPUT)
		{
			SDL_StopTextInput();
		}
		break;
	}
}

void Dialog::add(GuiElement * e)
{
	m_screen_elements.emplace_back(e);
	if (e->can_select())
		m_tab_items.emplace_back(e);
}

void Dialog::set_flags(uint16_t flags)
{
	m_flags = flags;
}

void Dialog::reload_lang(void)
{
	for (auto const& element : m_screen_elements)
	{
		element->refresh();
	}
}

const SDL_Point Dialog::position(void)
{
	return SDL_Point { m_dimensions.x, m_dimensions.y };
}

SDL_Helper * Dialog::helper(void)
{
	return m_helper;
}

int Dialog::get_left(void)
{
	return m_dimensions.x;
}

int Dialog::get_top(void)
{
	return m_dimensions.y;
}

int Dialog::get_right(void)
{
	return m_dimensions.x + m_dimensions.y;
}

int Dialog::get_bottom(void)
{
	return m_dimensions.y + m_dimensions.h;
}
