/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "config.hpp"
#include "util.hpp"
#include "../hook/gamepad_binding.hpp"
#include <util/config-file.h>

namespace io_config
{
    input_filter io_window_filters; /* Global filters */
    std::mutex filter_mutex;        /* Thread safety for writing/reading filters */

    bool control = false;
    bool remote = false;
    bool gamepad = true;
    bool uiohook = true;
    bool overlay = true;
    bool history = true;
    bool regex = false;
    bool log_flag = false;
    int filter_mode = 0;
    uint16_t refresh_rate = 250;
    uint16_t port = 1608;

    void set_defaults(config_t* cfg)
    {
        config_set_default_bool(cfg, S_REGION, S_UIOHOOK, io_config::uiohook);
        config_set_default_bool(cfg, S_REGION, S_GAMEPAD, io_config::gamepad);
        config_set_default_bool(cfg, S_REGION, S_OVERLAY, io_config::overlay);
        config_set_default_bool(cfg, S_REGION, S_HISTORY, io_config::history);

        config_set_default_bool(cfg, S_REGION, S_REMOTE, io_config::remote);
        config_set_default_bool(cfg, S_REGION, S_LOGGING, io_config::log_flag);
        config_set_default_int(cfg, S_REGION, S_PORT, io_config::port);
        config_set_default_int(cfg, S_REGION, S_REFRESH, io_config::refresh_rate);
        config_set_default_int(cfg, S_FILTER_MODE, S_REFRESH, io_config::filter_mode);

        /* Gamepad binding defaults */
#ifdef LINUX
        for (const auto &binding : gamepad::default_bindings)
            config_set_default_int(cfg, S_REGION, binding.setting, binding.default_value);
#endif
    }

    void load(config_t* cfg)
    {
        io_config::uiohook = config_get_bool(cfg, S_REGION, S_UIOHOOK);
        io_config::gamepad = config_get_bool(cfg, S_REGION, S_GAMEPAD);
        io_config::remote = config_get_bool(cfg, S_REGION, S_REMOTE);
        io_config::control = config_get_bool(cfg, S_REGION, S_CONTROL);
        io_config::filter_mode = config_get_int(cfg, S_REGION, S_FILTER_MODE);

        io_config::port = config_get_int(cfg, S_REGION, S_PORT);
        io_config::log_flag = config_get_bool(cfg, S_REGION, S_LOGGING);
        io_config::refresh_rate = config_get_int(cfg, S_REGION, S_REFRESH);
    }

    void save(config_t* cfg)
    {
        /* Window filters are directly saved in formAccept */
        config_set_bool(cfg, S_REGION, S_UIOHOOK, io_config::uiohook);
        config_set_bool(cfg, S_REGION, S_GAMEPAD, io_config::gamepad);
        config_set_bool(cfg, S_REGION, S_REMOTE, io_config::remote);
        config_set_bool(cfg, S_REGION, S_CONTROL, io_config::control);
        config_set_bool(cfg, S_REGION, S_HISTORY, io_config::history);
        config_set_bool(cfg, S_REGION, S_OVERLAY, io_config::overlay);
        config_set_int(cfg, S_REGION, S_PORT, io_config::port);
        config_set_int(cfg, S_REGION, S_REFRESH, io_config::refresh_rate);
        config_set_bool(cfg, S_REGION, S_LOGGING, io_config::log_flag);
        config_set_bool(cfg, S_REGION, S_REGEX, io_config::regex);
    }
}