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

#include "palette.hpp"

palette::palette()
{
    m_black = { 0, 0, 0, 255 };
    m_white = { 213, 213, 213, 255 };

    m_foreground = { 255, 255, 255, 255 };
    m_background = { 255, 255, 255, 255 };
    m_accent = { 64, 64, 64, 255 };

    m_tooltip_body = { 248, 230, 177, 128 };
    m_orange = { 255, 92, 38, 255 };

    m_red = { 240, 20, 20, 255 };
    m_green = { 33, 135, 55, 255 };
    m_blue = { 33, 118, 243, 255 };
    m_gray = { 55, 55, 55, 255 };
    m_dark_gray = { 45, 45, 45, 255 };
    m_light_gray = { 80, 80, 80, 255 };
}

palette::~palette() = default;

SDL_Color* palette::get_bg()
{
    return &m_background;
}

SDL_Color* palette::get_fg()
{
    return &m_foreground;
}

SDL_Color* palette::get_accent()
{
    return &m_accent;
}

SDL_Color* palette::white()
{
    return &m_white;
}

SDL_Color* palette::black()
{
    return &m_black;
}

SDL_Color* palette::gray()
{
    return &m_gray;
}

SDL_Color* palette::dark_gray()
{
    return &m_dark_gray;
}

SDL_Color* palette::light_gray()
{
    return &m_light_gray;
}

SDL_Color* palette::blue()
{
    return &m_blue;
}

SDL_Color* palette::red()
{
    return &m_red;
}

SDL_Color* palette::green()
{
    return &m_green;
}

SDL_Color* palette::orange()
{
    return &m_orange;
}

SDL_Color* palette::get_tooltip_body()
{
    return &m_tooltip_body;
}
