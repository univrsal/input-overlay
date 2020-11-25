/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
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
	io_config::instance = obs_frontend_get_global_config();
	CDEF_BOOL(S_UIOHOOK, io_config::uiohook);
	CDEF_BOOL(S_GAMEPAD, io_config::gamepad);
	CDEF_BOOL(S_OVERLAY, io_config::overlay);

	CDEF_BOOL(S_REMOTE, io_config::remote);
	CDEF_BOOL(S_LOGGING, io_config::log_flag);
	CDEF_INT(S_PORT, io_config::port);
	CDEF_INT(S_REFRESH, io_config::refresh_rate);
	CDEF_INT(S_REFRESH, io_config::filter_mode);

	/* Gamepad binding defaults */
	//    for (const auto &binding : gamepad::default_bindings)
	//        CDEF_INT(binding.setting, binding.default_value);
}

void load()
{
	/* Asure that ~/.config folder exists */
	if (!QDir::home().exists(".config") && !QDir::home().mkdir(".config"))
		berr("Couldn't create ~/.config, configuration files won't be saved");

	io_config::uiohook = CGET_BOOL(S_UIOHOOK);
	io_config::gamepad = CGET_BOOL(S_GAMEPAD);
	io_config::remote = CGET_BOOL(S_REMOTE);
	io_config::control = CGET_BOOL(S_CONTROL);
	io_config::filter_mode = CGET_INT(S_FILTER_MODE);

	io_config::port = CGET_INT(S_PORT);
	io_config::log_flag = CGET_BOOL(S_LOGGING);
	io_config::refresh_rate = CGET_INT(S_REFRESH);
}

void save()
{
	/* Window filters are directly saved in formAccept */
	CSET_BOOL(S_UIOHOOK, io_config::uiohook);
	CSET_BOOL(S_GAMEPAD, io_config::gamepad);
	CSET_BOOL(S_REMOTE, io_config::remote);
	CSET_BOOL(S_CONTROL, io_config::control);
	CSET_BOOL(S_OVERLAY, io_config::overlay);
	CSET_INT(S_PORT, io_config::port);
	CSET_INT(S_REFRESH, io_config::refresh_rate);
	CSET_BOOL(S_LOGGING, io_config::log_flag);
	CSET_BOOL(S_REGEX, io_config::regex);
}

}
