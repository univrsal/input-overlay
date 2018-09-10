/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include <SDL.h>

#define SDL_PAD_MASK        0x00FF0000 /* SDL uses 32bit integer for */
#define SDL_MOUSE_MASK      0x00FE0000 /* codes so there's plenty of space */
#define TO_PAD_MASK(a)      (a | SDL_PAD_MASK)
#define TO_MOUSE_MASK(a)    (a | SDL_MOUSE_MASK)

#define UTIL_MAX(a,b)               (((a) > (b)) ? (a) : (b))
#define UTIL_MIN(a,b)               (((a) < (b)) ? (a) : (b))
#define UTIL_CLAMP(lower, x, upper) (UTIL_MIN(upper, UTIL_MAX(x, lower)))

SDL_bool util_move_element(int* x, int* y, SDL_Keycode key);

class Timer
{
public:
    Timer() { start(); };

    void start()
    {
        m_start_ticks = SDL_GetTicks();
        m_end_ticks = 0;
        m_started = true;
    }

    void stop()
    {
        m_end_ticks = SDL_GetTicks();
        m_started = true;
    }

    bool started() const { return m_started; }

    uint32_t get_delta() const { return m_end_ticks - m_start_ticks; }

    uint32_t get_time() const { return SDL_GetTicks() - m_start_ticks; }
private:
    uint32_t m_start_ticks = 0;
    uint32_t m_end_ticks = 0;
    bool m_started = false;
};
