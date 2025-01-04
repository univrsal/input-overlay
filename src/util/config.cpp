/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.xyz/alex/input-overlay
 * Copyright 2023 univrsal <uni@vrsal.xyz>.
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
bool enable_input_control = false;
bool enable_websocket_server = false;
bool enable_gamepad_hook = true;
bool enable_uiohook = true;
bool enable_overlay_source = true;
bool regex = false;
bool log_flag = false;
int filter_mode = 0;
uint16_t server_refresh_rate = 250;
uint16_t server_port = 1608;
uint16_t wss_port = 16899;
std::string wss_bind_address;

void set_defaults()
{
    instance = obs_frontend_get_global_config();
    CDEF_BOOL(S_UIOHOOK, enable_uiohook);
    CDEF_BOOL(S_GAMEPAD, enable_gamepad_hook);
    CDEF_BOOL(S_OVERLAY, enable_overlay_source);

    CDEF_STR(S_WSS_ADDRESS, "0.0.0.0");
    CDEF_BOOL(S_LOGGING, log_flag);
    CDEF_INT(S_PORT, server_port);
    CDEF_INT(S_WSS_PORT, wss_port);
    CDEF_BOOL(S_ENABLE_WSS, enable_websocket_server);
    CDEF_INT(S_REFRESH, server_refresh_rate);
    CDEF_INT(S_REFRESH, filter_mode);
}

void load()
{
    enable_uiohook = CGET_BOOL(S_UIOHOOK);
    enable_gamepad_hook = CGET_BOOL(S_GAMEPAD);
    enable_websocket_server = CGET_BOOL(S_ENABLE_WSS);
    enable_input_control = CGET_BOOL(S_CONTROL);
    filter_mode = CGET_INT(S_FILTER_MODE);

    server_port = CGET_INT(S_PORT);
    wss_port = CGET_INT(S_WSS_PORT);
    log_flag = CGET_BOOL(S_LOGGING);
    server_refresh_rate = CGET_INT(S_REFRESH);
    wss_bind_address = CGET_STR(S_WSS_ADDRESS);
}

void save()
{
    /* Window filters are directly saved in formAccept */
    CSET_BOOL(S_UIOHOOK, enable_uiohook);
    CSET_BOOL(S_GAMEPAD, enable_gamepad_hook);
    CSET_BOOL(S_CONTROL, enable_input_control);
    CSET_BOOL(S_OVERLAY, enable_overlay_source);
    CSET_INT(S_PORT, server_port);
    CSET_INT(S_REFRESH, server_refresh_rate);
    CSET_BOOL(S_LOGGING, log_flag);
    CSET_BOOL(S_REGEX, regex);
    CSET_INT(S_WSS_PORT, wss_port);
    CSET_BOOL(S_ENABLE_WSS, enable_websocket_server);
    CSET_STR(S_WSS_ADDRESS, wss_bind_address.c_str());
}

}
