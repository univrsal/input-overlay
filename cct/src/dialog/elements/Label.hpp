/**
 * Created by univrsal on 14.07.2017.
 * This file is part of reloded which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/reloded
 */

#pragma once

#include "GuiElement.hpp"
#include "../Dialog.hpp"
#include <string>
#include <vector>
#include <memory>

#define LABEL_LINE_SPACE 2

class Dialog;

class GuiElement;

static const std::string NEW_LINE = "\n";

class Label :
	public GuiElement
{
public:
	Label(int8_t id, int x, int y, const char * text, Dialog * parent);
	Label(int8_t id, int x, int y, const char * text, Dialog * parent, SDL_Color * color);

	~Label();

	void close(void);

	void draw_background(void);

	void draw_foreground(void);

	bool handle_events(SDL_Event * event);

	void set_text(std::string text);
	
private:
	void format_text(std::string& s);

	std::vector<std::unique_ptr<std::string>> m_lines;

	SDL_Color *m_color;

	uint8_t m_type;
	uint8_t m_line_height;
};
