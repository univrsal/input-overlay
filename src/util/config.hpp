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

#pragma once

#include "input_filter.hpp"
#include <mutex>
#include <util/config-file.h>

#define CDEF_STR(id, value) config_set_default_string(io_config::instance, S_REGION, id, value)
#define CDEF_INT(id, value) config_set_default_int(io_config::instance, S_REGION, id, value)
#define CDEF_UINT(id, value) config_set_default_uint(io_config::instance, S_REGION, id, value)
#define CDEF_BOOL(id, value) config_set_default_bool(io_config::instance, S_REGION, id, value)

#define CGET_STR(id) config_get_string(io_config::instance, S_REGION, id)
#define CGET_INT(id) config_get_int(io_config::instance, S_REGION, id)
#define CGET_UINT(id) config_get_uint(io_config::instance, S_REGION, id)
#define CGET_BOOL(id) config_get_bool(io_config::instance, S_REGION, id)

#define CSET_STR(id, value) config_set_string(io_config::instance, S_REGION, id, value)
#define CSET_INT(id, value) config_set_int(io_config::instance, S_REGION, id, value)
#define CSET_UINT(id, value) config_set_uint(io_config::instance, S_REGION, id, value)
#define CSET_BOOL(id, value) config_set_bool(io_config::instance, S_REGION, id, value)

namespace io_config {
extern config_t *instance;

extern input_filter io_window_filters;
extern std::mutex filter_mutex;

/* Global boolean config values */
extern bool enable_input_control;
extern bool enable_gamepad_hook;
extern bool enable_uiohook;
extern bool enable_overlay_source;
extern bool enable_websocket_server;
extern bool regex;
extern int filter_mode;
/* Netowork config */
extern bool log_flag;
extern uint16_t server_refresh_rate;
extern uint16_t server_port;
extern uint16_t wss_port;
extern std::string wss_bind_address;

extern void set_defaults();

extern void load();

extern void save();
}
