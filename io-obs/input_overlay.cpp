/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include <obs-module.h>
#include <obs-frontend-api.h>
#include <QMainWindow>
#include <QAction>
#include <util/config-file.h>

#include "util/util.hpp"
#include "sources/input_source.hpp"
#include "sources/input_history.hpp"
#include "hook/hook_helper.hpp"
#include "hook/gamepad_hook.hpp"
#include "gui/io_settings_dialog.hpp"
#include "network/remote_connection.hpp"

#ifdef LINUX

extern void cleanupDisplay();

#endif

OBS_DECLARE_MODULE()

OBS_MODULE_USE_DEFAULT_LOCALE("input-overlay", "en-US")

void set_defaults(config_t* cfg)
{
    config_set_default_bool(cfg, S_REGION, S_IOHOOK, true);
    config_set_default_bool(cfg, S_REGION, S_GAMEPAD, true);
    config_set_default_bool(cfg, S_REGION, S_OVERLAY, true);
    config_set_default_bool(cfg, S_REGION, S_HISTORY, true);

    config_set_default_bool(cfg, S_REGION, S_REMOTE, false);
    config_set_default_bool(cfg, S_REGION, S_LOGGING, false);
    config_set_default_int(cfg, S_REGION, S_PORT, 1608);
    config_set_default_int(cfg, S_REGION, S_REFRESH, network::refresh_rate);

    /* Gamepad binding defaults */
#ifdef LINUX
    for (const auto& binding : gamepad::default_bindings)
        config_set_default_int(cfg, S_REGION, binding.setting, binding.default_value);
#endif
}

bool obs_module_load()
{

    auto cfg = obs_frontend_get_global_config();
    set_defaults(cfg);

    if (config_get_bool(cfg, S_REGION, S_HISTORY))
        sources::register_history();

    if (config_get_bool(cfg, S_REGION, S_OVERLAY))
        sources::register_overlay_source();

    const auto iohook = config_get_bool(cfg, S_REGION, S_IOHOOK);
    const auto gamepad = config_get_bool(cfg, S_REGION, S_GAMEPAD);
    const auto remote = config_get_bool(cfg, S_REGION, S_REMOTE);
    const auto control = config_get_bool(cfg, S_REGION, S_CONTROL);

    if (iohook || gamepad)
        hook::init_data_holder();

    if (iohook)
        hook::start_hook();

    if (gamepad)
        gamepad::start_pad_hook();

    if (remote) {
        const uint16_t port = config_get_int(cfg, S_REGION, S_PORT);
        network::local_input = gamepad || iohook;
        network::log_flag = config_get_bool(cfg, S_REGION, S_LOGGING);
        network::start_network(port);
        network::refresh_rate = config_get_int(cfg, S_REGION, S_REFRESH);
    }

    /* Input filtering via focused window title */
    if (control)
        io_config::io_window_filters.read_from_config(cfg);

    /* UI registration from
    * https://github.com/Palakis/obs-websocket/
    */
    const auto menu_action = static_cast<QAction*>(obs_frontend_add_tools_menu_qaction(T_MENU_OPEN_SETTINGS));
    obs_frontend_push_ui_translation(obs_module_get_string);
    const auto main_window = static_cast<QMainWindow*>(obs_frontend_get_main_window());
    settings_dialog = new io_settings_dialog(main_window);
    obs_frontend_pop_ui_translation();

    const auto menu_cb = []
    { settings_dialog->toggleShowHide(); };
    QAction::connect(menu_action, &QAction::triggered, menu_cb);

    return true;
}

void obs_module_unload()
{
    if (gamepad::gamepad_hook_state)
        gamepad::end_pad_hook();

    if (hook::hook_initialized)
        hook::end_hook();
#ifdef LINUX
    //cleanupDisplay();
#endif
}
