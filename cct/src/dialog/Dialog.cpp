#include "Dialog.hpp"
/**
 * Created by universallp on 23.05.2018.
 * This file is part of input-overlay which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

Dialog::Dialog(SDL_helper * sdl, SDL_Rect size, std::string title)
{
	m_helper = sdl;
	m_dimensions = size;
	m_title = title;
	m_title_bar = { m_dimensions.x + 5, m_dimensions.y + 5, m_dimensions.w - 10, 20 };
}

Dialog::Dialog(SDL_helper * sdl, SDL_Point size, std::string title)
{
	SDL_Point window_size = sdl->util_window_size();
	SDL_Rect temp = { window_size.x / 2 - size.x / 2, window_size.y / 2 - size.y / 2, size.x, size.y };
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
	m_screen_elements.emplace_back(new Label(0, m_dimensions.x + 8 , m_dimensions.y + 8, m_title.c_str(), this));
}

void Dialog::draw_background(void)
{
	// Dialog box
	m_helper->util_fill_rect_shadow(&m_dimensions, m_helper->palette()->get_accent());
	m_helper->util_draw_rect(&m_dimensions, m_helper->palette()->black());

	// Dialog titlebar
	m_helper->util_fill_rect_shadow(&m_dimensions, m_helper->palette()->light_gray());
}

void Dialog::draw_foreground(void)
{
	std::vector<std::unique_ptr<GuiElement>>::iterator iterator;

	for (iterator = m_screen_elements.begin(); iterator != m_screen_elements.end(); iterator++) {
		iterator->get()->draw_foreground();
	}
}

void Dialog::close(void)
{
	m_screen_elements.clear();
}

void Dialog::handle_events(SDL_Event * event)
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
		}
	}

	std::vector<std::unique_ptr<GuiElement>>::iterator iterator;

	for (iterator = m_screen_elements.begin(); iterator != m_screen_elements.end(); iterator++)
	{
		iterator->get()->handle_events(event);
	}
}

const SDL_Point Dialog::position(void)
{
	return SDL_Point { m_dimensions.x, m_dimensions.y };
}

SDL_helper * Dialog::helper(void)
{
	return m_helper;
}
