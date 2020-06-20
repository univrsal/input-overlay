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

#include "gui_element.hpp"
#include "../../util/sdl_helper.hpp"
#include "../dialog.hpp"

#if DEBUG_DRAW_OUTLINE
#include "../../util/palette.hpp"
#endif

gui_element::~gui_element()
{
	gui_element::close();
	m_parent_dialog = nullptr;
}

void gui_element::close()
{
	m_element_id = 0;
	m_dimensions = {0, 0, 0, 0};
}

bool gui_element::can_select()
{
	return false;
}

void gui_element::select_state(bool state)
{
	// NO-OP
}

void gui_element::resize()
{
	//NO-OP
}

void gui_element::refresh()
{
	/* NO OP */
}

dialog *gui_element::get_parent() const
{
	return m_parent_dialog;
}

sdl_helper *gui_element::get_helper() const
{
	return get_parent()->helper();
}

SDL_Rect *gui_element::get_dimensions()
{
	m_dimensions.x = get_left();
	m_dimensions.y = get_top();
	return &m_dimensions;
}

int8_t gui_element::get_id() const
{
	return m_element_id;
}

void gui_element::init(dialog *parent, const SDL_Rect dim, const int8_t id)
{
	m_parent_dialog = parent;
	m_dimensions = dim;

	m_position.x = dim.x;
	m_position.y = dim.y;

	m_element_id = id;
}

bool gui_element::is_mouse_over(const SDL_Point *p)
{
	if (!p)
		p = m_parent_dialog->helper()->util_mouse_pos();
	return sdl_helper::util_is_in_rect(get_dimensions(), p->x, p->y);
}

void gui_element::set_pos(const int x, const int y)
{
	m_dimensions.x = m_parent_dialog->position().x + x;
	m_dimensions.y = m_parent_dialog->position().y + y;
	m_position.x = x;
	m_position.y = y;
	resize();
}

uint8_t gui_element::get_cursor()
{
	return CURSOR_ARROW;
}

void gui_element::set_flags(const uint16_t flags)
{
	m_flags = flags;
}

int gui_element::get_left() const
{
	return m_flags & ELEMENT_ABSOLUTE_POSITION ? m_position.x : m_position.x + get_parent()->position().x;
}

int gui_element::get_right() const
{
	return get_left() + m_dimensions.w;
}

int gui_element::get_top() const
{
	return m_flags & ELEMENT_ABSOLUTE_POSITION ? m_position.y : m_position.y + get_parent()->position().y;
}

int gui_element::get_bottom() const
{
	return get_top() + m_dimensions.h;
}

int gui_element::get_width() const
{
	return m_dimensions.w;
}

int gui_element::get_height() const
{
	return m_dimensions.h;
}

void gui_element::draw_foreground()
{
#if _DEBUG
#if DEBUG_DRAW_OUTLINE
	const SDL_Rect *r = get_dimensions();
	get_helper()->util_draw_rect(get_dimensions(), get_helper()->get_palette()->black());
#endif
#endif
}

void gui_element::set_dim(const SDL_Rect r)
{
	m_dimensions = r;
	resize();
}
