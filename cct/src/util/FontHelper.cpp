/**
 * Created by universal on 19.07.17.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "FontHelper.hpp"

FontHelper::FontHelper(SDL_Helper * renderer)
{
    m_helper = renderer;
    m_mode = FONT_BLENDED;
}

FontHelper::~FontHelper()
{
    m_helper = nullptr;
}

void FontHelper::draw(const std::string *text, int x, int y, TTF_Font *font, const SDL_Color *fg, const SDL_Color *bg)
{
    SDL_Surface *surface = nullptr;
    SDL_Texture *texture = nullptr;

    switch (m_mode)
    {
    default:
    case FONT_BLENDED:
        surface = TTF_RenderUTF8_Blended(font, text->c_str(), *fg);
        break;
    case FONT_SHADED:
        surface = TTF_RenderUTF8_Shaded(font, text->c_str(), *fg, *bg);
        break;
    }

    if (surface)
    {
        texture = SDL_CreateTextureFromSurface(m_helper->renderer(), surface);

        if (texture)
        {
            SDL_Rect dest = { x, y, surface->w, surface->h };
            SDL_RenderCopy(m_helper->renderer(), texture, nullptr, &dest);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        }
        else
        {
            printf(SDL_TEXT_TO_TEXTURE, TTF_GetError());
        }
    }
    else
    {
        printf(SDL_TEXT_TO_SURFACE, TTF_GetError());
    }
}

void FontHelper::set_mode(int m)
{
    m_mode = m;
}

void FontHelper::draw(const std::string *text, int x, int y, TTF_Font *font)
{
    draw(text, x, y, font, m_helper->palette()->black(), m_helper->palette()->white());
}

void FontHelper::draw_rot(const std::string * text, int x, int y, TTF_Font * font, const SDL_Color * fg, double angle)
{
    SDL_Surface *surface = nullptr;
    SDL_Texture *texture = nullptr;

    surface = TTF_RenderUTF8_Blended(font, text->c_str(), *fg);

    if (surface)
    {
        texture = SDL_CreateTextureFromSurface(m_helper->renderer(), surface);

        if (texture)
        {
            SDL_Rect dest = { x, y, surface->w, surface->h };
            SDL_Point rot = { 0, 0 };
            SDL_RenderCopyEx(m_helper->renderer(), texture, nullptr, &dest, angle, &rot, SDL_FLIP_NONE);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        }
        else
        {
            printf(SDL_TEXT_TO_TEXTURE, TTF_GetError());
        }
    }
    else
    {
        printf(SDL_TEXT_TO_SURFACE, TTF_GetError());
    }
}

SDL_Rect FontHelper::get_text_dimension(TTF_Font *font, const std::string *text)
{
    if (text->empty())
    {
        return SDL_Rect { 0, 0, 0, 0 };
    }

    SDL_Surface *surface = nullptr;
    surface = TTF_RenderUTF8_Shaded(font, text->c_str(), *m_helper->palette()->black(),
        *m_helper->palette()->white());
    SDL_Rect dest = {};

    if (surface)
    {
        dest.w = surface->w;
        dest.h = surface->h;
    }
    else
    {
        printf(SDL_TEXT_TO_TEXTURE, TTF_GetError());
    }
    SDL_FreeSurface(surface);
    return dest;
}

void FontHelper::draw(const std::string *text, int x, int y, TTF_Font *font, const SDL_Color *fg)
{
    set_mode(FONT_BLENDED);
    draw(text, x, y, font, fg, nullptr);
}
