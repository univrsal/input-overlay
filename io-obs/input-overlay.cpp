#include <obs-module.h>

#include "util/util.hpp"
#include "sources/input-source.hpp"
#include "sources/input-history.hpp"

#ifdef LINUX
#include "hook/gamepad-hook.hpp"
#endif


/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("input-overlay", "en-US")

bool obs_module_load(void)
{
	Sources::register_history();
	Sources::register_overlay_source();
	Hook::start_hook();

	start_pad_hook();

	return true;
}

void obs_module_unload(void)
{
	if (Hook::hook_initialized)
		Hook::end_hook();

#ifdef LINUX
	if (gamepad_hook_state)
		end_pad_hook();
#endif
}
