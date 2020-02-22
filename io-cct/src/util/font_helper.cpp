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

#include "font_helper.hpp"
#include "constants.hpp"
#include "palette.hpp"

font_helper::font_helper(sdl_helper* renderer)
{
    m_helper = renderer;
    m_mode = FONT_SHADED;
}

font_helper::~font_helper()
{
    m_helper = nullptr;
}

void font_helper::draw(const std::string* text, const int x, const int y, TTF_Font* font, const SDL_Color* fg,
    const SDL_Color* bg, const uint8_t scale) const
{
    SDL_Surface* surface = nullptr;
    SDL_Texture* texture = nullptr;

    switch (m_mode) {
    default:
    case FONT_BLENDED:
        surface = TTF_RenderUTF8_Blended(font, text->c_str(), *fg);
        break;
    case FONT_SHADED:
        surface = TTF_RenderUTF8_Shaded(font, text->c_str(), *fg, *bg);
        break;
    case FONT_SOLID:
        surface = TTF_RenderUTF8_Solid(font, text->c_str(), *fg);
    }

    if (surface) {
        texture = SDL_CreateTextureFromSurface(m_helper->renderer(), surface);

        if (texture) {
            SDL_Rect dest = { x, y, surface->w * scale, surface->h * scale };
            SDL_RenderCopy(m_helper->renderer(), texture, nullptr, &dest);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        } else {
            printf(SDL_TEXT_TO_TEXTURE, TTF_GetError());
        }
    } else {
        printf(SDL_TEXT_TO_SURFACE, TTF_GetError());
    }
}

void font_helper::set_mode(const int m)
{
    m_mode = m;
}

void font_helper::draw(const std::string* text, const int x, const int y, TTF_Font* font, const uint8_t scale) const
{
    draw(text, x, y, font, m_helper->get_palette()->black(), m_helper->get_palette()->white(), scale);
}

void font_helper::draw_rot(const std::string* text, const int x, const int y, TTF_Font* font, const SDL_Color* fg,
    const double angle) const
{
    SDL_Surface* surface = nullptr;
    SDL_Texture* texture = nullptr;

    surface = TTF_RenderUTF8_Blended(font, text->c_str(), *fg);

    if (surface) {
        texture = SDL_CreateTextureFromSurface(m_helper->renderer(), surface);

        if (texture) {
            SDL_Rect dest = { x, y, surface->w, surface->h };
            SDL_Point rot = { 0, 0 };
            SDL_RenderCopyEx(m_helper->renderer(), texture, nullptr, &dest, angle, &rot, SDL_FLIP_NONE);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        } else {
            printf(SDL_TEXT_TO_TEXTURE, TTF_GetError());
        }
    } else {
        printf(SDL_TEXT_TO_SURFACE, TTF_GetError());
    }
}

SDL_Rect font_helper::get_text_dimension(TTF_Font* font, const std::string* text) const
{
    if (text->empty()) {
        return SDL_Rect { 0, 0, 0, 0 };
    }

    SDL_Surface* surface = nullptr;
    surface = TTF_RenderUTF8_Shaded(font, text->c_str(), *m_helper->get_palette()->black(),
        *m_helper->get_palette()->white());
    SDL_Rect dest = {};

    if (surface) {
        dest.w = surface->w;
        dest.h = surface->h;
    } else {
        printf(SDL_TEXT_TO_TEXTURE, TTF_GetError());
    }
    SDL_FreeSurface(surface);
    return dest;
}

void font_helper::draw(const std::string* text, const int x, const int y, TTF_Font* font, const SDL_Color* fg,
    const uint8_t scale)
{
    set_mode(FONT_BLENDED);
    draw(text, x, y, font, fg, nullptr, scale);
}
