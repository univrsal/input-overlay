/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include <stdint.h>

namespace gamepad
{
	/* Contains the current state of a gamepad*/
	class gamepad_state
	{
	public:
		gamepad_state();
		void get_difference(const gamepad_state* new_state, gamepad_state* result);
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