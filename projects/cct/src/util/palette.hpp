/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
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

#include <SDL.h>

class palette {
public:
    palette();

    ~palette();

    SDL_Color *get_bg();

    SDL_Color *get_fg();

    SDL_Color *get_accent();

    SDL_Color *get_tooltip_body();

    SDL_Color *white();

    SDL_Color *black();

    SDL_Color *gray();

    SDL_Color *dark_gray();

    SDL_Color *light_gray();

    SDL_Color *blue();

    SDL_Color *red();

    SDL_Color *green();

    SDL_Color *orange();

private:
    SDL_Color m_background{};
    SDL_Color m_foreground{};
    SDL_Color m_accent{};

    SDL_Color m_orange{};

    SDL_Color m_white{};
    SDL_Color m_black{};
    SDL_Color m_light_gray{};

    SDL_Color m_gray{};
    SDL_Color m_dark_gray{};

    SDL_Color m_blue{};
    SDL_Color m_red{};
    SDL_Color m_green{};

    SDL_Color m_tooltip_body{};
};
