/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2019 univrsal <universailp@web.de>.
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
#include "xinput_fix.hpp"
#include <cstdint>

namespace gamepad
{
    /* Contains the current state of a gamepad*/
    class gamepad_state
    {
    public:
        gamepad_state();
#ifdef _WIN32
        explicit gamepad_state(xinput_fix::gamepad* pad);
#else
        /* Linux constructor */
#endif
        bool merge(gamepad_state* new_state);

		int16_t button_states;
		/* Floats take more space, but are
         * more straightforward than using
         * 16 bit/8 bit depending on the system
         */
		float stick_l_x, stick_l_y;
		float stick_r_x, stick_r_y;
		int8_t trigger_l, trigger_r;
	};

} /* namespace gamepad */
