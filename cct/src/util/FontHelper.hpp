/**
 * Created by universal on 19.07.17.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#define FONT_SOLID 0
#define FONT_SHADED 1
#define FONT_BLENDED 2

#define FONT_SMALL 10
#define FONT_DEFAULT 15
#define FONT_LARGE 18

#define FONT_ROBOTO_SMALL 0
#define FONT_ROBOTO_LARGE 1
#define FONT_WSTRING 2

#ifndef RELODED_FONT_H
#define RELODED_FONT_H

#include <SDL_ttf.h>
#include <string>
#include "SDL_helper.hpp"
#include "Constants.hpp"

class SDL_helper;

class FontHelper
{
public:
    FontHelper(SDL_helper *renderer);

    ~FontHelper();

    void set_mode(int m);

    void draw(const std::string * text, int x, int y, TTF_Font * font, const SDL_Color * fg, const SDL_Color * bg);

    void draw(const std::string * text, int x, int y, TTF_Font * font, const SDL_Color * fg);

    void draw(const std::string * text, int x, int y, TTF_Font * font);

    void draw_rot(const std::string * text, int x, int y, TTF_Font * font, const SDL_Color * fg, double angle);

    SDL_Rect get_text_dimension(TTF_Font *font, const std::string *text);

private:
	SDL_helper *m_helper;
	uint8_t m_mode;
};


#endif //RELODED_FONT_H
