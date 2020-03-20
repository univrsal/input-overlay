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
 *
 * based on https://forums.tigsource.com/index.php?PHPSESSID=biq1psqu44jbmc8if73olk4fh6&topic=26792.msg848190#msg848190
 * Thanks Microsoft for not allowing access to the guide button
 *************************************************************************/

#pragma once

#include <layout_constants.h>
#include <keycodes.h>

namespace xinput_fix {

struct pad_handle;

extern bool loaded;

/* Util functions */
void load();

void unload();

bool pressed(pad_handle *pad, gamepad::code c);

void get_dpad(pad_handle *pad, dpad_direction dirs[2]);

pad_handle *create();

void free(pad_handle *pad);

int update(pad_handle *pad, uint8_t id);

float trigger_l(pad_handle *pad);

float trigger_r(pad_handle *pad);

float stick_l_x(pad_handle *pad);

float stick_l_y(pad_handle *pad);

float stick_r_x(pad_handle *pad);

float stick_r_y(pad_handle *pad);
}
