/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include <obs-module.h>

#include "util/util.hpp"
#include "sources/input_source.hpp"
#include "sources/input_history.hpp"
#include "hook/hook_helper.hpp"
#include "hook/gamepad_hook.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("input-overlay", "en-US")

bool obs_module_load()
{
    sources::register_history();
    sources::register_overlay_source();
    hook::start_hook();

    gamepad::start_pad_hook();

    return true;
}

void obs_module_unload()
{
    if (gamepad::gamepad_hook_state)
        gamepad::end_pad_hook();
    
    if (hook::hook_initialized)
        hook::end_hook();
}
