/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <universailp@web.de>.
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

#define FONT_SOLID 0
#define FONT_SHADED 1
#define FONT_BLENDED 2

#define FONT_SMALL 10
#define FONT_DEFAULT 16
#define FONT_LARGE 28

#define FONT_WSTRING 0
#define FONT_WSTRING_LARGE 1

#include "sdl_helper.hpp"
#include <SDL_ttf.h>
#include <string>

class sdl_helper;

class font_helper {
public:
	font_helper(sdl_helper *renderer);

	~font_helper();

	void set_mode(int m);

	void draw(const std::string *text, int x, int y, TTF_Font *font, const SDL_Color *fg, const SDL_Color *bg,
			  uint8_t scale = 1) const;

	void draw(const std::string *text, int x, int y, TTF_Font *font, const SDL_Color *fg, uint8_t scale = 1);

	void draw(const std::string *text, int x, int y, TTF_Font *font, uint8_t scale = 1) const;

	void draw_rot(const std::string *text, int x, int y, TTF_Font *font, const SDL_Color *fg, double angle) const;

	SDL_Rect get_text_dimension(TTF_Font *font, const std::string *text) const;

private:
	sdl_helper *m_helper;
	uint8_t m_mode;
};
