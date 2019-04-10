/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once
#include <cstdint>
#include "xinput_fix.hpp"

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
