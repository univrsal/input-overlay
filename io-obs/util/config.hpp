/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

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