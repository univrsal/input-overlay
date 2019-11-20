/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2019 univrsal <universailp@web.de>.
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

#include "checkbox.hpp"
#include "../../util/palette.hpp"

checkbox::checkbox(const int8_t id, const int x, const int y, const char *text, dialog *parent)
{
	SDL_Rect temp = {x, y, 0, 0};
	m_checkbox = {x, y, CHECKBOX_SIZE, CHECKBOX_SIZE};

	m_label = new label(id, x + CHECKBOX_SIZE + 5, y, text, parent);

	if (m_label->get_height() > CHECKBOX_SIZE) {
		m_checkbox.y = m_label->get_height() / 2 - CHECKBOX_SIZE / 2;
		temp.h = m_label->get_height();
	} else {
		m_label->set_pos(x + CHECKBOX_SIZE + 5, y + CHECKBOX_SIZE / 2 - m_label->get_height() / 2);
		temp.h = CHECKBOX_SIZE;
	}

	temp.w = m_label->get_width() + CHECKBOX_SIZE + 5;
	gui_element::init(parent, temp, id);
}

checkbox::checkbox(const int8_t id, const int x, const int y, const char *text, dialog *parent, const bool state)
    : checkbox(id, x, y, text, parent)
{
    m_state = state;
}

checkbox::~checkbox()
{
    checkbox::close();
}

void checkbox::close()
{
	delete m_label;
	m_label = nullptr;
}

bool checkbox::can_select()
{
	return true;
}

void checkbox::select_state(const bool state)
{
	m_focused = state;
}

void checkbox::draw_background()
{
	m_label->draw_background();

	get_helper()->util_fill_rect(get_left(), m_parent_dialog->get_top() + m_checkbox.y, CHECKBOX_SIZE, CHECKBOX_SIZE,
	                             get_helper()->get_palette()->dark_gray());
	get_helper()->util_draw_rect(get_left(), m_parent_dialog->get_top() + m_checkbox.y, CHECKBOX_SIZE, CHECKBOX_SIZE,
	                             get_helper()->get_palette()->light_gray());
	if (m_focused) {
		get_helper()->util_draw_rect(get_left() - 2, get_top() - 2, get_width() + 4, get_height() + 4,
		                             get_helper()->get_palette()->light_gray());
	}
}

void checkbox::draw_foreground()
{
	gui_element::draw_foreground();
	m_label->draw_foreground();
	if (m_state) {
		get_helper()->util_fill_rect(get_left() + 4, m_parent_dialog->get_top() + m_checkbox.y + 4, CHECKBOX_SIZE - 8,
		                             CHECKBOX_SIZE - 8, get_helper()->get_palette()->green());
	}
}

bool checkbox::handle_events(SDL_Event *event, bool was_handled)
{
	auto handled = false;
	if (event->type == SDL_MOUSEBUTTONDOWN) {
		if (event->button.button == SDL_BUTTON_LEFT && is_mouse_over()) {
			handled = true;
			m_state = !m_state;
			m_parent_dialog->change_focus(m_element_id);
			get_parent()->action_performed(get_id());
		}
	} else if (m_focused && event->type == SDL_KEYUP) {
		if (event->key.keysym.sym == SDLK_RETURN) {
			m_state = !m_state;
			get_parent()->action_performed(get_id());
		}
	}
	return handled;
}

void checkbox::set_checked(const bool state)
{
	m_state = state;
}
