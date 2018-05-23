/**
 * Created by universallp on 26.07.2017.
 * This file is part of reloded which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/reloded
 */

#include "Label.hpp"
#include "../../util/SDL_helper.hpp"

Label::Label(int8_t id, int x, int y, const char *text, Dialog *parent)
{
	SDL_Rect temp = parent->helper()->util_text_dim(&m_text);
	init(parent, temp, id);

	m_text = std::string(text);
	temp.x = x;
	temp.y = y;

	m_color = get_helper()->palette()->get_accent();
}

Label::~Label()
{
	close();
	m_text.clear();
}

void Label::close(void)
{
}

void Label::draw_background(void)
{
	if (!m_text.empty())
	{
		get_helper()->util_text(&m_text, m_parent_dialog->position().x +
			m_dimensions.x, m_parent_dialog->position().y +
			m_dimensions.y, m_color);
	}
}

void Label::handle_events(SDL_Event *event)
{

}

void Label::draw_foreground(void)
{
	GuiElement::draw_foreground();
}

void Label::set_text(std::string text)
{
	m_text = text;
	SDL_Rect temp = get_helper()->util_text_dim(&text);
	m_dimensions.w = temp.w;
	m_dimensions.h = temp.h;
}
