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

#include <SDL.h>
#include <string>
#include <json.hpp>
#define SDL_PAD_MASK 0x00FF0000   /* SDL uses 32bit integer for */
#define SDL_MOUSE_MASK 0x00FE0000 /* codes so there's plenty of space */
#define TO_PAD_MASK(a) (a | SDL_PAD_MASK)
#define TO_MOUSE_MASK(a) (a | SDL_MOUSE_MASK)

#define SDL_PAD_MASK 0x00FF0000   /* SDL uses 32bit integer for */
#define SDL_MOUSE_MASK 0x00FE0000 /* codes so there's plenty of space */
#define TO_PAD_MASK(a) (a | SDL_PAD_MASK)
#define TO_MOUSE_MASK(a) (a | SDL_MOUSE_MASK)

#define UTIL_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define UTIL_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define UTIL_CLAMP(lower, x, upper)(UTIL_MIN(upper, UTIL_MAX(x, lower)))

SDL_bool
util_move_element(int *x, int *y, SDL_Keycode key);

using json = nlohmann::json;

class timer {
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

namespace util {
extern void replace(std::string &str, const char *find, const char *replace);
extern bool load_json(const std::string &path, json &out);
extern bool is_empty(const std::string &path);
extern bool can_access(const std::string &path);
}
