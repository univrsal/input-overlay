/**
 * Created by universal on 8/1/17.
 * This file is part of reloded which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/reloded
 */


#ifndef RELODED_TEXTBOX_H
#define RELODED_TEXTBOX_H


#include "./GuiElement.hpp"
#include "../Dialog.hpp"
#include <string>

class Dialog;

class GuiElement;

class Textbox : public GuiElement
{
public:
	Textbox(int8_t id, int x, int y, int w, int h, std::string text, Dialog *parent);

	virtual ~Textbox();

	void close(void);

	void draw_background(void);

	void draw_foreground(void);

	void handle_events(SDL_Event *event);

	void set_text(std::string s);

	void append_text(std::string s);

private:
	std::string m_text;
	std::string m_composition;
	std::string m_cut_text;

	bool m_focused;
};


#endif //RELODED_TEXTBOX_H
