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

#include <QAction>
#include <QMainWindow>
#include <obs-frontend-api.h>
#include <obs-module.h>
#include <util/config-file.h>

#include "gui/io_settings_dialog.hpp"
#include "hook/gamepad_hook.hpp"
#include "hook/hook_helper.hpp"
#include "network/remote_connection.hpp"
#include "sources/input_history.hpp"
#include "sources/input_source.hpp"
#include "util/config.hpp"
#include "util/lang.h"
#include "util/util.hpp"

#ifdef LINUX

extern void cleanupDisplay();

#endif

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("input-overlay", "en-US")

bool obs_module_load()
{
    io_config::set_defaults();
    io_config::load();

    if (io_config::history)
        sources::register_history();
    if (io_config::overlay)
        sources::register_overlay_source();

    if (io_config::uiohook || io_config::gamepad)
        hook::init_data_holder();

    if (io_config::uiohook)
        hook::start_hook();

    if (io_config::gamepad)
        gamepad::start_pad_hook();

    if (io_config::remote) {
        network::local_input = io_config::gamepad || io_config::uiohook;
        network::start_network(io_config::port);
    }

    /* Input filtering via focused window title */
    if (io_config::control)
        io_config::io_window_filters.read_from_config();

    /* UI registration from
    * https://github.com/Palakis/obs-websocket/
    */
    const auto menu_action = static_cast<QAction*>(obs_frontend_add_tools_menu_qaction(T_MENU_OPEN_SETTINGS));
    obs_frontend_push_ui_translation(obs_module_get_string);
    const auto main_window = static_cast<QMainWindow*>(obs_frontend_get_main_window());
    settings_dialog = new io_settings_dialog(main_window);
    obs_frontend_pop_ui_translation();

    const auto menu_cb = [] { settings_dialog->toggleShowHide(); };
    QAction::connect(menu_action, &QAction::triggered, menu_cb);

    return true;
}

void obs_module_unload()
{
    /* Save config values again */
    io_config::save();

    if (gamepad::gamepad_hook_state)
        gamepad::end_pad_hook();

    if (hook::hook_initialized)
        hook::end_hook();

#ifdef LINUX
    cleanupDisplay();
#endif
}
