/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2021 univrsal <uni@vrsal.de>.
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

#include "config.hpp"
#include "settings.h"
#include "log.h"
#include <QDir>
#include <obs-frontend-api.h>

namespace io_config {
config_t *instance = nullptr;
input_filter io_window_filters; /* Global filters */
std::mutex filter_mutex;        /* Thread safety for writing/reading filters */
bool use_dinput = false;
bool use_js = true;
bool control = false;
bool remote = false;
bool gamepad = true;
bool uiohook = true;
bool overlay = true;
bool regex = false;
bool log_flag = false;
int filter_mode = 0;
uint16_t refresh_rate = 250;
uint16_t port = 1608;

void set_defaults()
{
    instance = obs_frontend_get_global_config();
    CDEF_BOOL(S_UIOHOOK, io_config::uiohook);
    CDEF_BOOL(S_GAMEPAD, io_config::gamepad);
    CDEF_BOOL(S_OVERLAY, io_config::overlay);

    CDEF_BOOL(S_REMOTE, io_config::remote);
    CDEF_BOOL(S_LOGGING, io_config::log_flag);
    CDEF_INT(S_PORT, io_config::port);
    CDEF_INT(S_REFRESH, io_config::refresh_rate);
    CDEF_INT(S_REFRESH, io_config::filter_mode);
    CDEF_BOOL(S_USE_DINPUT, io_config::use_dinput);
    CDEF_BOOL(S_USE_JS, io_config::use_dinput);
}

void load()
{
    /* Asure that ~/.config folder exists */
    if (!QDir::home().exists(".config") && !QDir::home().mkdir(".config"))
        berr("Couldn't create ~/.config, configuration files won't be saved");

    uiohook = CGET_BOOL(S_UIOHOOK);
    gamepad = CGET_BOOL(S_GAMEPAD);
    remote = CGET_BOOL(S_REMOTE);
    control = CGET_BOOL(S_CONTROL);
    filter_mode = CGET_INT(S_FILTER_MODE);

    port = CGET_INT(S_PORT);
    log_flag = CGET_BOOL(S_LOGGING);
    refresh_rate = CGET_INT(S_REFRESH);
    use_dinput = CGET_BOOL(S_USE_DINPUT);
    use_js = CGET_BOOL(S_USE_JS);
}

void save()
{
    /* Window filters are directly saved in formAccept */
    CSET_BOOL(S_UIOHOOK, uiohook);
    CSET_BOOL(S_GAMEPAD, gamepad);
    CSET_BOOL(S_REMOTE, remote);
    CSET_BOOL(S_CONTROL, control);
    CSET_BOOL(S_OVERLAY, overlay);
    CSET_INT(S_PORT, port);
    CSET_INT(S_REFRESH, refresh_rate);
    CSET_BOOL(S_LOGGING, log_flag);
    CSET_BOOL(S_REGEX, regex);
    CSET_BOOL(S_USE_DINPUT, use_dinput);
    CSET_BOOL(S_USE_JS, use_js);
}

}
