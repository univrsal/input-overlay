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

#pragma once

#include "../dialog.hpp"
#include "gui_element.hpp"
#include <memory>
#include <string>
#include <vector>

class dialog;

class gui_element;

class label : public gui_element {
public:
	label(int8_t id, int x, int y, const char *text, dialog *parent, uint16_t flags = 0x0);

	label(int8_t id, int x, int y, const char *text, uint8_t font, dialog *parent, uint16_t flags = 0x0);

	label(int8_t id, int x, int y, const char *text, dialog *parent, SDL_Color *color);

	~label();

	void close() override;

	void draw_background() override;

	void draw_foreground() override;

	bool handle_events(SDL_Event *event, bool was_handled) override;

	void set_text(std::string text);

	void refresh() override;

	void set_font(const uint8_t font) { m_font = UTIL_CLAMP(FONT_WSTRING, font, FONT_WSTRING_LARGE); }

private:
	std::string m_unlocalized_text;
	std::vector<std::unique_ptr<std::string>> m_lines;

	SDL_Color *m_color;

	uint8_t m_font = FONT_WSTRING;
};
