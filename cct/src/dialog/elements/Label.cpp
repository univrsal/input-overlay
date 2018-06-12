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
	SDL_Rect temp{ x, y, 0, 0 }; /* Width/Height will be calculated by set_text */
	init(parent, temp, id);

	set_text(std::string(text));

	m_color = get_helper()->palette()->white();
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
		for (auto it = m_lines.begin(); it != m_lines.end(); ++it)
		{
			if (!it->get()->empty())
			{
				get_helper()->util_text(it->get(), get_left(),
					get_top() + ((LABEL_LINE_SPACE + m_line_height) * i),
					m_color);
			}
			i++;
		}
	}
}

void Label::draw_foreground(void)
{
	GuiElement::draw_foreground();

}

bool Label::handle_events(SDL_Event * event)
{
	return false;
}

void Label::set_text(std::string text)
{
	format_text(text);
}

void Label::format_text(std::string& s)
{
	if (s.empty())
		return;

	SDL_Rect dim = { 0, 0, 0, 0 };
	int width = 0;
	int lines = 1;

	auto start = 0U;
	auto end = s.find(NEW_LINE);
	std::string token;

	while (end != std::string::npos) {
		token = s.substr(start, end - start);
		m_lines.push_back(std::unique_ptr<std::string>(new std::string(token)));
		start = end + NEW_LINE.length();
		end = s.find(NEW_LINE, start);

		if (!token.empty())
			dim = get_helper()->util_text_dim(&token);

		if (dim.w > width)
			width = dim.w;
		lines++;
	}
	token = s.substr(start, end);
	m_lines.push_back(std::unique_ptr<std::string>(new std::string(token)));

	if (!token.empty())
		dim = get_helper()->util_text_dim(&token);

	if (dim.w > width)
		width = dim.w;

	m_line_height = dim.h;
	m_dimensions.h = ((dim.h + LABEL_LINE_SPACE) * lines) - LABEL_LINE_SPACE; // Last line space doesn't count
	m_dimensions.w = width;
}
