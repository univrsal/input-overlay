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

#pragma once

#include <stdint.h>

class element_data_holder;

namespace gamepad {
struct binding {
	const char *setting;
	const char *text_box_id;
	uint8_t default_value;
	bool axis_event; /* true if axis event */
};


class gamepad_binding {

public:
	gamepad_binding();

    virtual void init_default();
    virtual void set_binding(uint8_t id, uint8_t binding, bool axis_event) = 0;
};
}
