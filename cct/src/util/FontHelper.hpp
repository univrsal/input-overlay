/**
 * Created by universal on 19.07.17.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#define FONT_SOLID          0
#define FONT_SHADED         1
#define FONT_BLENDED        2

#define FONT_SMALL          10
#define FONT_DEFAULT        16
#define FONT_LARGE          18

#define FONT_WSTRING   0
#define FONT_WSTRING_LARGE  1

#include <SDL_ttf.h>
#include <string>
#include "SDL_Helper.hpp"
#include "Constants.hpp"

class SDL_Helper;

class FontHelper
{
public:
    FontHelper(SDL_Helper* renderer);

    ~FontHelper();

    void set_mode(int m);

    void draw(const std::string* text, int x, int y, TTF_Font* font, const SDL_Color* fg, const SDL_Color* bg, uint8_t scale = 1) const;

    void draw(const std::string* text, int x, int y, TTF_Font* font, const SDL_Color* fg, uint8_t scale = 1);

    void draw(const std::string* text, int x, int y, TTF_Font* font, uint8_t scale = 1) const;

    void draw_rot(const std::string* text, int x, int y, TTF_Font* font, const SDL_Color* fg, double angle) const;

    SDL_Rect get_text_dimension(TTF_Font* font, const std::string* text) const;

private:
    SDL_Helper* m_helper;
    uint8_t m_mode;
};
