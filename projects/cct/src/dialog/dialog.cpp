/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#include "dialog.hpp"
#include "../util/palette.hpp"
#include <utility>

dialog::dialog(sdl_helper *sdl, const SDL_Rect size, const std::string &title) : m_offset_x(0)
{
	m_helper = sdl;
	m_dimensions = size;
	m_title = title;
	m_title_bar = {m_dimensions.x + 5, m_dimensions.y + 5, m_dimensions.w - 10, 20};
}

dialog::dialog(sdl_helper *sdl, const SDL_Point size, const std::string &title) : m_offset_x(0)
{
	const auto window_size = sdl->util_window_size();
	const SDL_Rect temp = {(*window_size).x / 2 - size.x / 2, (*window_size).y / 2 - size.y / 2, size.x, size.y};
	m_helper = sdl;
	m_dimensions = temp;
	m_title = title;
	m_title_bar = {m_dimensions.x + 5, m_dimensions.y + 5, m_dimensions.w - 10, 20};
}

dialog::~dialog()
{
	dialog::close();
}

void dialog::init()
{
	add(new label(8, 6, m_title.c_str(), this));

	const auto temp = m_helper->util_window_size();

	if (m_flags & DIALOG_FLUID) {
		m_dimensions.w = temp->x - FLUID_BORDER;
		m_dimensions.h = temp->y - FLUID_BORDER;
		m_dimensions.x = temp->x / 2 - m_dimensions.w / 2;
		m_dimensions.y = temp->y / 2 - m_dimensions.h / 2;
	}

	if (m_flags & DIALOG_CENTERED) {
		m_dimensions.x = temp->x / 2 - m_dimensions.w / 2;
		m_dimensions.y = temp->y / 2 - m_dimensions.h / 2;
	}
	m_title_bar = {m_dimensions.x + 5, m_dimensions.y + 5, m_dimensions.w - 10, 20};
}

void dialog::draw_background()
{
	if (m_flags & DIALOG_TOP_MOST)
	/* Tint background in gray */
	{
		const auto s = m_helper->util_window_size();
		SDL_Rect temp = {0, 0, s->x, s->y};
		m_helper->util_fill_rect(&temp, m_helper->get_palette()->dark_gray(), 200);
	}

	// Dialog box
	m_helper->util_fill_rect_shadow(&m_dimensions, m_helper->get_palette()->get_accent());
	m_helper->util_draw_rect(&m_dimensions, m_helper->get_palette()->dark_gray());

	// Dialog title bar
	m_helper->util_fill_rect(&m_title_bar, m_helper->get_palette()->light_gray());

	for (auto const &element : m_screen_elements) {
		if (m_focused_element && element->get_id() == m_focused_element->get_id())
			continue;
		element->draw_background();
	}

	if (m_focused_element) /* Focused element has top priority */
		m_focused_element->draw_background();
}

void dialog::draw_foreground()
{
	for (auto const &element : m_screen_elements) {
		if (m_focused_element && element->get_id() == m_focused_element->get_id())
			continue;
		element->draw_foreground();
	}

	if (m_focused_element) /* Focused element has top priority */
		m_focused_element->draw_foreground();
}

void dialog::close()
{
	m_tab_items.clear();
	m_screen_elements.clear();
}

bool dialog::handle_events(SDL_Event *event)
{
	auto was_handled = false;

	if (m_flags & DIALOG_DRAGGABLE) {
		if (event->type == SDL_MOUSEBUTTONDOWN) {
			if (event->button.button == SDL_BUTTON_LEFT) {
				if (sdl_helper::util_is_in_rect(&m_title_bar, event->button.x, event->button.y)) {
					m_is_dragging = true;
					m_offset_x = event->button.x - m_title_bar.x;
					m_offset_y = event->button.y - m_title_bar.y;
					was_handled = true;
				}

				if (sdl_helper::util_is_in_rect(&m_dimensions, event->button.x, event->button.y)) {
					action_performed(ACTION_FOCUSED);
					was_handled = true;
				}
			}
		} else if (event->type == SDL_MOUSEBUTTONUP) {
			if (event->button.button == SDL_BUTTON_LEFT) {
				m_is_dragging = false;
			}
		} else if (event->type == SDL_MOUSEMOTION) {
			if (m_is_dragging) {
				m_dimensions.x = event->button.x - m_offset_x;
				m_dimensions.y = event->button.y - m_offset_y;
				m_title_bar = {m_dimensions.x + 5, m_dimensions.y + 5, m_dimensions.w - 10, 20};
				was_handled = true;
			}
		} else if (event->type == SDL_KEYDOWN) {
			if (event->key.keysym.sym == SDLK_RETURN) {
				action_performed(ACTION_OK);
				was_handled = true;
			}
		}
	}

	if (event->type == SDL_WINDOWEVENT) {
		if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
			/*
				Assure that dialogs aren't outside of
				the main window after resizing
			*/

			const auto temp = m_helper->util_window_size();

			if (temp->x < get_right()) {
				m_dimensions.x = temp->x - m_dimensions.w;
			}

			if (temp->y < get_bottom()) {
				m_dimensions.y = temp->y - m_dimensions.h;
			}

			if (m_flags & DIALOG_FLUID) {
				m_dimensions.w = temp->x - FLUID_BORDER;
				m_dimensions.h = temp->y - FLUID_BORDER;
				m_dimensions.x = temp->x / 2 - m_dimensions.w / 2;
				m_dimensions.y = temp->y / 2 - m_dimensions.h / 2;
			}

			if (m_flags & DIALOG_CENTERED) {
				m_dimensions.x = temp->x / 2 - m_dimensions.w / 2;
				m_dimensions.y = temp->y / 2 - m_dimensions.h / 2;
			}

			m_title_bar = {m_dimensions.x + 5, m_dimensions.y + 5, m_dimensions.w - 10, 20};

			for (const auto &element : m_screen_elements)
				element->resize();
		}
	} else if (event->type == SDL_KEYDOWN) {
		if (event->key.keysym.sym == SDLK_TAB) {
			for (auto const &e : m_tab_items) {
				e->select_state(false);
			}

			m_focused_element_id += m_helper->is_shift_down() ? -1 : 1;

			if (m_focused_element_id < 0)
				m_focused_element_id = static_cast<int16_t>(m_tab_items.size()) - 1;
			else if (m_focused_element_id > static_cast<int16_t>(m_tab_items.size()) - 1)
				m_focused_element_id = 0;

			m_tab_items[m_focused_element_id]->select_state(true);
			change_focus(m_tab_items[m_focused_element_id]->get_id());
		}
	}

	auto cursor_handled = false;
	auto element_handled = false;

	if (m_focused_element) {
		if (m_focused_element->handle_events(event, element_handled))
			element_handled = true;

		if (m_focused_element->is_mouse_over()) {
			cursor_handled = true;
			m_helper->set_cursor(m_focused_element->get_cursor());
		}
	}

	for (auto const &element : m_screen_elements) {
		if (m_focused_element && element->get_id() == m_focused_element->get_id())
			continue;

		if (element->handle_events(event, element_handled))
			element_handled = true;

		if (!cursor_handled && element->is_mouse_over()) {
			cursor_handled = true;
			m_helper->set_cursor(element->get_cursor());
		}
	}

	if (!cursor_handled)
		m_helper->set_cursor(CURSOR_ARROW);
	return was_handled || element_handled;
}

void dialog::action_performed(const int8_t action_id)
{
	switch (action_id) {
	case ACTION_FOCUSED:
		if (m_flags & DIALOG_TEXTINPUT)
			SDL_StartTextInput();
		break;
	case ACTION_UNFOCUSED:
		if (m_flags & DIALOG_TEXTINPUT)
			SDL_StopTextInput();
		break;
	default:;
	}
}

void dialog::add(gui_element *e)
{
	m_screen_elements.emplace_back(e);
	if (e->can_select())
		m_tab_items.emplace_back(e);
}

void dialog::set_dimension(const uint16_t w, const uint16_t h)
{
	m_dimensions.w = w;
	m_dimensions.h = h;
	m_title_bar = {m_dimensions.x + 5, m_dimensions.y + 5, m_dimensions.w - 10, 20};
}

void dialog::set_flags(const uint16_t flags)
{
	m_flags = flags;
}

void dialog::reload_lang()
{
	for (auto const &element : m_screen_elements) {
		element->refresh();
	}
}

void dialog::change_focus(const int8_t id)
{
	auto index = 0;
	for (auto e : m_tab_items) {
		if (e->get_id() == id) {
			m_focused_element_id = index;
			m_focused_element = e;
			break;
		}
		index++;
	}
}

SDL_Point dialog::position() const
{
	return SDL_Point{m_dimensions.x, m_dimensions.y};
}

sdl_helper *dialog::helper() const
{
	return m_helper;
}

int dialog::get_left() const
{
	return m_dimensions.x;
}

int dialog::get_top() const
{
	return m_dimensions.y;
}

int dialog::get_right() const
{
	return m_dimensions.x + m_dimensions.y;
}

int dialog::get_bottom() const
{
	return m_dimensions.y + m_dimensions.h;
}
