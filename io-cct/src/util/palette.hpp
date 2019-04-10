/**
 * Created by univrsal on 13.07.2017.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <SDL.h>

class palette
{
public:
    palette();

    ~palette();

    SDL_Color* get_bg();

    SDL_Color* get_fg();

    SDL_Color* get_accent();

    SDL_Color* get_tooltip_body();

    SDL_Color* white();

    SDL_Color* black();

    SDL_Color* gray();

    SDL_Color* dark_gray();

    SDL_Color* light_gray();

    SDL_Color* blue();

    SDL_Color* red();

    SDL_Color* green();

    SDL_Color* orange();

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
