/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2019 univrsal <universailp@web.de>.
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

#include "input_filter.hpp"
#include <mutex>
#define DEBUG_LOG(lvl, msg, ...) if (io_config::log_flag) blog(lvl, "[input-overlay] " msg, ##__VA_ARGS__)

namespace io_config
{
    extern input_filter io_window_filters;
    extern std::mutex filter_mutex;

    /* Global boolean config values */
    extern bool control;
    extern bool remote;
    extern bool gamepad;
    extern bool uiohook;
    extern bool overlay;
    extern bool history;
    extern bool regex;
    extern int filter_mode;
    /* Netowork config */
    extern bool log_flag;
    extern uint16_t refresh_rate;
    extern uint16_t port;

    extern void set_defaults(config_t* cfg);

    extern void load(config_t* cfg);

    extern void save(config_t* cfg);
}
