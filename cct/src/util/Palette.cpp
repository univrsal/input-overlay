/**
 * Created by universallp on 13.07.2017.
 * This file is part of input-overlay which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */


#include "Palette.hpp"

Palette::Palette()
{
    m_black = {0, 0, 0, 255};
    m_white = {255, 255, 255, 255};

    m_foreground = {255, 255, 255, 255};
    m_background = {255, 255, 255, 255};
    m_accent = {64, 64, 64, 255};

    m_tooltip_body = {248, 230, 177, 128};

    m_blue = {33, 118, 243};
    m_gray = {55, 55, 55, 255};
    m_dark_gray = {45, 45, 45, 255};
    m_light_gray = { 80, 80, 80, 255};
}

Palette::~Palette()
{

}

SDL_Color *Palette::get_bg(void)
{
    return &m_background;
}

SDL_Color *Palette::get_fg(void)
{
    return &m_foreground;
}

SDL_Color *Palette::get_accent(void)
{
    return &m_accent;
}

SDL_Color *Palette::white(void)
{
    return &m_white;
}

SDL_Color *Palette::black(void)
{
    return &m_black;
}

SDL_Color * Palette::gray(void)
{
    return &m_gray;
}

SDL_Color * Palette::dark_gray(void)
{
    return &m_dark_gray;
}

SDL_Color * Palette::blue(void)
{
    return &m_blue;
}

SDL_Color *Palette::get_tooltip_body(void)
{
    return &m_tooltip_body;
}
