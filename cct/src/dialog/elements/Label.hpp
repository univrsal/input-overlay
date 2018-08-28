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

class Dialog;

class GuiElement;

class Label :
    public GuiElement
{
public:
    Label(int8_t id, int x, int y, const char * text, Dialog * parent, uint16_t flags = 0x0);
    Label(int8_t id, int x, int y, const char * text, uint8_t font, Dialog * parent, uint16_t flags = 0x0);
    Label(int8_t id, int x, int y, const char * text, Dialog * parent, SDL_Color * color);

    ~Label();

    void close(void) override;

    void draw_background(void) override;

    void draw_foreground(void) override;

    bool handle_events(SDL_Event * event, bool was_handled) override;

    void set_text(std::string text);

    void refresh(void) override;

    void set_font(uint8_t font) { m_font = UTIL_CLAMP(FONT_ROBOTO_SMALL, font, FONT_WSTRING); }
private:
    std::string m_unlocalized_text;
    std::vector<std::unique_ptr<std::string>> m_lines;

    SDL_Color *m_color;

    uint8_t m_font = FONT_ROBOTO_SMALL;
};
