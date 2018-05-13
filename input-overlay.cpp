#include <obs-module.h>

#include "util/util.hpp"
#include "sources/input-source.hpp"
#include "sources/input-history.hpp"

#ifdef LINUX
#include "hook/gamepad-hook.hpp"
#endif


/**
 * This file is part of input-overlay
 * which is licenced under the MIT licence.
 * See LICENCE or https://mit-license.org
 * github.com/univrsal/input-overlay
 */

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("input-overlay", "en-US")

bool obs_module_load(void)
{
	register_history();
	register_overlay_source();

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
