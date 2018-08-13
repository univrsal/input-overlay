/**
 * Created by univrsal on on 11.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "Combobox.hpp"
Combobox::Combobox(int8_t id, int x, int y, int w, int h, Dialog * parent, uint16_t flags)
{
	SDL_Rect temp = { x, y, w, h };
	m_flags = flags;
	init(parent, temp, id);

}

void Combobox::close(void)
{
	m_items.clear();
}

void Combobox::draw_background(void)
{
	get_helper()->util_fill_rect(get_dimensions(), get_helper()->palette()->gray());
	if (m_focused)
	{
		get_helper()->util_draw_rect(get_dimensions(), get_helper()->palette()->light_gray());

		if (m_list_open)
		{
			SDL_Rect temp = { get_left(), get_bottom() - 1, get_width(),
				m_items.size() * (get_helper()->util_font_height(FONT_WSTRING) + ITEM_V_SPACE) + ITEM_V_SPACE };
			get_helper()->util_fill_rect(&temp, get_helper()->palette()->gray());
			get_helper()->util_draw_rect(&temp, get_helper()->palette()->light_gray());

			temp.y = get_bottom() + ITEM_V_SPACE + m_hovered_id * (get_helper()->util_font_height(FONT_WSTRING) + ITEM_V_SPACE);
			temp.h = get_helper()->util_font_height(FONT_WSTRING);
			get_helper()->util_fill_rect(&temp, get_helper()->palette()->light_gray());
		}
	}
	else
	{
		get_helper()->util_draw_rect(get_dimensions(), get_helper()->palette()->dark_gray());
	}
}

void Combobox::draw_foreground(void)
{
	GuiElement::draw_foreground();

	if (!m_items.empty() && m_selected_id >= 0 && m_selected_id < m_items.size())
		get_helper()->util_text(&m_items[m_selected_id], get_left() + 2, get_top() + 2, get_helper()->palette()->white(), FONT_WSTRING);

	if (m_list_open)
	{
		uint16_t y = ITEM_V_SPACE;
		for (auto const& element : m_items)
		{
			get_helper()->util_text(&element, get_left() + 2, get_bottom() + y, get_helper()->palette()->white(), FONT_WSTRING);
			y += get_helper()->util_font_height(FONT_WSTRING) + ITEM_V_SPACE;
		}
	}
}

bool Combobox::can_select(void)
{
	return true;
}

void Combobox::select_state(bool state)
{
	m_focused = state;
}

bool Combobox::handle_events(SDL_Event * event)
{
	bool was_handled = false;
	if (event->type == SDL_MOUSEBUTTONDOWN)
	{
		/* Handle focus */
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			m_focused = is_mouse_over(event->button.x, event->button.y);
			if (m_focused)
			{
				was_handled = true;
				m_list_open = !m_list_open;
				m_hovered_id = 0;
			}
			else
			{
				m_list_open = false;
			}
		}
	}
	else if (m_list_open && event->type == SDL_MOUSEMOTION)
	{
		
	}
	else if (event->type == SDL_KEYDOWN)
	{
		if (m_focused && event->key.keysym.sym == SDLK_RETURN)
		{
			if (m_list_open)
			{
				m_selected_id = m_hovered_id;
				m_parent_dialog->action_performed(ACTION_COMBO_ITEM_SELECTED);
			}
			m_list_open = !m_list_open;
			m_hovered_id = 0;
		}
		else if (m_list_open || m_focused)
		{
			if (event->key.keysym.sym == SDLK_UP)
				cycle_up(!m_list_open);
			else if (event->key.keysym.sym == SDLK_DOWN)
				cycle_down(!m_list_open);
		}
	}
	return was_handled;
}

void Combobox::cycle_up(bool select)
{
	if (m_hovered_id - 1 < 0)
		m_hovered_id = m_items.size() - 1;
	else
		m_hovered_id--;
	if (select)
	{
		m_selected_id = m_hovered_id;
		m_parent_dialog->action_performed(ACTION_COMBO_ITEM_SELECTED);
	}
}

void Combobox::cycle_down(bool select)
{
	if (m_hovered_id + 1 >= m_items.size())
		m_hovered_id = 0;
	else
		m_hovered_id++;
	if (select)
	{
		m_selected_id = m_hovered_id;
		m_parent_dialog->action_performed(ACTION_COMBO_ITEM_SELECTED);
	}
}
