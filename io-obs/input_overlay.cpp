/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include <obs-module.h>
#include <obs-frontend-api.h>
#include <QAction>
#include <QMainWindow>
#include <QTimer>

#include "util/util.hpp"
#include "sources/input_source.hpp"
#include "sources/input_history.hpp"
#include "hook/hook_helper.hpp"
#include "hook/gamepad_hook.hpp"
#include "gui/io_settings_dialog.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("input-overlay", "en-US")

io_settings_dialog* dialog;

bool obs_module_load()
{
    /* UI registration from
     * https://github.com/Palakis/obs-websocket/
     */

    auto menu_action = static_cast<QAction*>(obs_frontend_add_tools_menu_qaction(
        T_MENU_OPEN_SETTINGS));
    obs_frontend_push_ui_translation(obs_module_get_string);
    const auto main_window = static_cast<QMainWindow*>(obs_frontend_get_main_window());
    dialog = new io_settings_dialog(main_window);
    obs_frontend_pop_ui_translation();

    const auto menu_cb = [] {
        dialog->toggleShowHide();
    };
    menu_action->connect(menu_action, &QAction::triggered, menu_cb);

    sources::register_history();
    sources::register_overlay_source();
    hook::start_hook();
    gamepad::start_pad_hook();

    return true;
}

void obs_module_unload()
{
    if (gamepad::gamepad_hook_state)
        gamepad::end_pad_hook();
    
    if (hook::hook_initialized)
        hook::end_hook();
}
