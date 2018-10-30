/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "gamepad_state.hpp"
#include "xinput_fix.hpp"
#include "gamepad.hpp"

namespace gamepad
{
	gamepad_state::gamepad_state()
	{
		button_states = 0x0;
		stick_l_x = stick_l_y = 0.f;
		stick_r_x = stick_r_y = 0.f;
		trigger_l = trigger_r = 0;
	}

	void gamepad_state::get_difference(const gamepad_state* new_state, gamepad_state* result)
	{
		if (!result)
			return;

		result->button_states = new_state->button_states; /* New states have priority */

        


	}
}
