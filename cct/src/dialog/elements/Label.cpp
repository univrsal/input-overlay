/**
 * Created by univrsal on 26.07.2017.
 * This file is part of reloded which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/reloded
 */

#include "Label.hpp"
#include "../../util/SDL_Helper.hpp"
#include "../../util/Localization.hpp"

Label::Label(int8_t id, int x, int y, const char *text, Dialog *parent, uint16_t flags)
{
	SDL_Rect temp { x, y, 0, 0 }; /* Width/Height will be calculated by set_text */
	init(parent, temp, id);
	m_flags = flags;
	m_unlocalized_text = text;
	refresh();
	m_color = get_helper()->palette()->white();
}

Label::Label(int8_t id, int x, int y, const char * text, uint8_t font, Dialog * parent, uint16_t flags)
	: Label(id, x, y, text, parent, flags)
{
	m_font = font;
}

Label::Label(int8_t id, int x, int y, const char * text, Dialog * parent, SDL_Color * color)
	: Label(id, x, y, text, parent)
{
	m_color = color;
}

Label::~Label()
{
	close();
}

void Label::close(void)
{
	m_lines.clear();
}

void Label::draw_background(void)
{
	if (!m_lines.empty())
	{
		int i = 0;
		int y = 0;
		for (auto const &line : m_lines)
		{
			if (!line->empty())
			{
				get_helper()->util_text(line.get(), get_left(),
					get_top() + y,
					m_color, m_font);
			}
			y += LINE_SPACE + get_helper()->util_font_height(m_font);
			i++;
		}
	}
}

void Label::draw_foreground(void)
{
	GuiElement::draw_foreground();
}

bool Label::handle_events(SDL_Event * event, bool was_handled)
{
	return false;
}

void Label::set_text(std::string text)
{
	if (!text.empty())
	{
		m_lines.clear();
		get_helper()->format_text(&text, m_lines, m_dimensions);
	}
	else
	{
		m_lines.clear();
	}
}

void Label::refresh(void)
{
	if (m_flags & ELEMENT_UNLOCALIZED)
	{
		set_text(m_unlocalized_text);
	}
	else
	{
		set_text(get_helper()->loc(m_unlocalized_text.c_str()));
		if (get_helper()->localization()->is_roman())
		{
			m_font = FONT_ROBOTO_SMALL;
		}
		else
		{
			m_font = FONT_WSTRING;
		}
	}
}
