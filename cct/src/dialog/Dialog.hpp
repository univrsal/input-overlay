/**
 * Created by universallp on 14.07.2017.
 * This file is part of input-overlay which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "../util/SDL_helper.hpp"
#include <string>

class SDL_helper;

class Dialog
{
public:

    virtual void init(SDL_helper *sdl) = 0;

    virtual void draw_background(void) = 0;

    virtual void draw_foreground(void) = 0;

    virtual void close(void) = 0;

    virtual void handle_events(void) = 0;

    virtual void action_performed(int8_t action_id) = 0;

    virtual void set_active_tooltip(std::string *text, uint16_t x, uint16_t y) = 0;

    SDL_helper *m_helper;
};
