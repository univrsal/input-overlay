/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "gamepad_state.hpp"
#include "xinput_fix.hpp"
#include "../../io-obs/util/layout_constants.hpp"
#define IO_CLIENT 1 /* Prevents external util.hpp from including obs headers */
#include "../../io-obs/util/util.hpp"

namespace gamepad
{
	gamepad_state::gamepad_state()
	{
		button_states = 0x0;
		stick_l_x = stick_l_y = 0.f;
		stick_r_x = stick_r_y = 0.f;
		trigger_l = trigger_r = 0;
	}

    gamepad_state::gamepad_state(xinput_fix::gamepad* pad)
    {
        if (pad)
        {
            button_states = pad->wButtons;
            
            stick_l_x = pad->sThumbLX / STICK_MAX_VAL;
            stick_l_y = pad->sThumbLY / STICK_MAX_VAL;
            stick_r_x = pad->sThumbRX / STICK_MAX_VAL;
            stick_r_y = pad->sThumbRY / STICK_MAX_VAL;

            /* Both linux and windows use one byte for these */
            trigger_l = pad->bLeftTrigger;
            trigger_r = pad->bRightTrigger;
        }
    }

    bool gamepad_state::merge(gamepad_state* new_state)
    {
		if (!new_state)
			return false;
        auto merged = false;
#ifdef _WIN32 
	    /* On windows the new state contains all changes -> No merging needed*/
        if (new_state->button_states != button_states)
            merged = true;
        /* TODO: Maybe use deadzone here 
         * Right now even the slightest movement results in updates
         */
        else if (new_state->stick_l_x != stick_l_x) 
            merged = true;                           
        else if (new_state->stick_l_y != stick_l_y)
            merged = true;
        else if (new_state->stick_r_x != stick_r_x)
            merged = true;
        else if (new_state->stick_r_y != stick_r_y)
            merged = true;
        else if (new_state->trigger_l != trigger_l)
            merged = true;
        else if (new_state->trigger_r != trigger_r)
            merged = true;

        if (merged)
        {
            button_states = new_state->button_states;
            stick_l_x = new_state->stick_l_x;
            stick_l_y = new_state->stick_l_y;
            stick_r_x = new_state->stick_r_x;
            stick_r_y = new_state->stick_r_y;
            trigger_l = new_state->trigger_l;
            trigger_r = new_state->trigger_r;
        }
#else   /* On linux old values should be kept and if needed overridden by new ones */

        /*TODO: implementation*/
#endif

		return merged;
	}
}
