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

#include "label.hpp"
#include "../../util/localization.hpp"
#include "../../util/palette.hpp"
#include "../../util/sdl_helper.hpp"

label::label(const int x, const int y, const char *text, dialog *parent, uint16_t flags)
{
	const SDL_Rect temp{x, y, 0, 0}; /* Width/Height will be calculated by set_text */
	gui_element::init(parent, temp);
	m_flags = flags;
	m_unlocalized_text = text;
	label::refresh();
	m_color = get_helper()->get_palette()->white();
}

label::label(const int x, const int y, const char *text, const uint8_t font, dialog *parent, const uint16_t flags)
	: label(x, y, text, parent, flags)
{
	m_font = font;
}

label::label(const int x, const int y, const char *text, dialog *parent, SDL_Color *color) : label(x, y, text, parent)
{
	m_color = color;
}

label::~label()
{
	label::close();
}

void label::close()
{
	m_lines.clear();
}

void label::draw_background()
{
	if (!m_lines.empty()) {
		auto i = 0;
		auto y = 0;
		for (auto const &line : m_lines) {
			if (!line->empty()) {
				get_helper()->util_text(line.get(), get_left(), get_top() + y, m_color, m_font);
			}
			y += LINE_SPACE + get_helper()->util_default_text_height();
			i++;
		}
	}
}

void label::draw_foreground()
{
	gui_element::draw_foreground();
}

bool label::handle_events(SDL_Event *event, bool was_handled)
{
	return false;
}

void label::set_text(std::string text)
{
	if (!text.empty()) {
		m_lines.clear();
		get_helper()->format_text(&text, m_lines, m_dimensions);
	} else {
		m_lines.clear();
	}
}

void label::refresh()
{
	if (m_flags & ELEMENT_UNLOCALIZED) {
		set_text(m_unlocalized_text);
	} else {
		set_text(get_helper()->loc(m_unlocalized_text.c_str()));
	}
}
