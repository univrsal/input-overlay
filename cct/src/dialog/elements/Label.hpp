/**
 * Created by universallp on 14.07.2017.
 * This file is part of reloded which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/reloded
 */

#pragma once

#include "GuiElement.hpp"
#include "../Dialog.hpp"
#include "../../util/SDL_helper.hpp"
#include <string>

#define LABEL_TITLE 0
#define LABEL_GOLD 1
#define LABEL_WHITE 2
#define LABEL_WHITE_SHADOW 3
#define LABEL_DEFAULT 4

class Dialog;

class GuiElement;

class Label :
public GuiElement
{
public:
    Label(int8_t id, int x, int y, const char *text, Dialog *parent);

    ~Label();

    void close(void);

    void draw_background(void);

    void draw_foreground(void);

    void handle_events(SDL_Event *event);

    void set_text(std::string text);

    std::string m_text;

    SDL_Color *m_color;
private:
	uint8_t m_type;
};
