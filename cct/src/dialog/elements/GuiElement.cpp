/**
* Created by universallp on 14.07.2017.
* This file is part of input-overlay which is licenced
* under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
* github.com/univrsal/input-overlay
*/

#include "GuiElement.hpp"

GuiElement::~GuiElement()
{
    close();
    m_parent_dialog = NULL;
}

void GuiElement::close(void)
{
    m_element_id = 0;
    m_dimensions = {0, 0, 0, 0};
    m_scaled_dimensions = {0, 0, 0, 0};
}

void GuiElement::resize(void)
{
	// NO-OP
}

Dialog *GuiElement::get_parent()
{
    return m_parent_dialog;
}

SDL_helper * GuiElement::get_helper()
{
    return get_parent()->m_helper;
}

const SDL_Rect *GuiElement::get_dimensions()
{
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
    m_scaled_dimensions = dim;
    m_element_id = id;
}

bool GuiElement::is_mouse_over(const int & x, const int & y)
{
    return x > get_dimensions()->x && x < get_dimensions()->x + get_dimensions()->w
           && y > get_dimensions()->y && y < get_dimensions()->y + get_dimensions()->h;
}

void GuiElement::set_pos(const int& x, const int& y)
{
    m_dimensions.x = x;
    m_dimensions.y = y;
}

int GuiElement::get_left()
{
    return m_dimensions.x;
}

int GuiElement::get_right()
{
    return m_dimensions.x + m_dimensions.w;
}

int GuiElement::get_top()
{
    return m_dimensions.y;
}

int GuiElement::get_bottom()
{
    return m_dimensions.y + m_dimensions.h;
}

void GuiElement::draw_foreground(void)
{
    if (DEBUG_DRAW_OUTLINE)
    {
        const SDL_Rect * r = get_dimensions();
        get_helper()->util_draw_rect(r);
    }
}

void GuiElement::set_dim(SDL_Rect r)
{
    m_dimensions = r;
    resize();
}
