/**
 * Created by univrsal on 14.07.2017.
 * This file is part of reloded which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/reloded
 */

#pragma once

#include "gui_element.hpp"
#include "../dialog.hpp"
#include <string>
#include <vector>
#include <memory>

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

	void set_font(const uint8_t font)
	{
		m_font = UTIL_CLAMP(FONT_WSTRING, font, FONT_WSTRING_LARGE);
	}

private:
	std::string m_unlocalized_text;
	std::vector<std::unique_ptr<std::string>> m_lines;

	SDL_Color *m_color;

	uint8_t m_font = FONT_WSTRING;
};
