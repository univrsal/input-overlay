/**
* Created by univrsal on 14.07.2017.
* This file is part of input-overlay which is licensed
* under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
* github.com/univrsal/input-overlay
*/

#include "GuiElement.hpp"
#include "../../util/SDL_helper.hpp"
#include "../Dialog.hpp"

GuiElement::~GuiElement()
{
	close();
	m_parent_dialog = nullptr;
}

void GuiElement::close(void)
{
	m_element_id = 0;
	m_dimensions = { 0, 0, 0, 0 };
}

bool GuiElement::can_select(void)
{
	return false;
}

void GuiElement::select_state(bool state)
{
	// NO-OP
}

void GuiElement::resize(void)
{
	//NO-OP
}

void GuiElement::refresh(void)
{
	/* NO OP */
}

Dialog *GuiElement::get_parent()
{
	return m_parent_dialog;
}

SDL_helper * GuiElement::get_helper()
{
	return get_parent()->helper();
}

SDL_Rect * GuiElement::get_dimensions()
{
	m_dimensions.x = get_left();
	m_dimensions.y = get_top();
	return &m_dimensions;
}

int8_t GuiElement::get_id()
{
	return m_element_id;
}

void GuiElement::init(Dialog *parent, SDL_Rect dim, int8_t id)
{
	m_parent_dialog = parent;
	m_dimensions = dim;

	m_position.x = dim.x;
	m_position.y = dim.y;

	m_element_id = id;
}

bool GuiElement::is_mouse_over(const int & x, const int & y)
{
	return get_helper()->util_is_in_rect(get_dimensions(), x, y);
}

void GuiElement::set_pos(int x, int y)
{
	m_dimensions.x = m_parent_dialog->position().x + x;
	m_dimensions.y = m_parent_dialog->position().y + y;
	m_position.x = x;
	m_position.y = y;
	resize();
}

uint8_t GuiElement::get_cursor(void)
{
	return CURSOR_ARROW;
}

void GuiElement::set_flags(uint8_t flags)
{
	m_flags = flags;
}

int GuiElement::get_left()
{
	return m_flags & ELEMENT_ABSOLUTE_POSITION ? m_position.x : m_position.x + get_parent()->position().x;
}

int GuiElement::get_right()
{
	return get_left() + m_dimensions.w;
}

int GuiElement::get_top()
{
	return m_flags & ELEMENT_ABSOLUTE_POSITION ? m_position.y : m_position.y + get_parent()->position().y;
}

int GuiElement::get_bottom()
{
	return get_top() + m_dimensions.h;
}

int GuiElement::get_width(void)
{
	return m_dimensions.w;
}

int GuiElement::get_height(void)
{
	return m_dimensions.h;
}

void GuiElement::draw_foreground(void)
{
#if _DEBUG
#if DEBUG_DRAW_OUTLINE
	const SDL_Rect * r = get_dimensions();
	printf("X. %i, Y. %i, W. %i, H. %i \n", r->x, r->y, r->w, r->w);
	get_helper()->util_draw_rect(get_dimensions(), get_helper()->palette()->black());
#endif
#endif
}

void GuiElement::set_dim(SDL_Rect r)
{
	m_dimensions = r;
	resize();
}
