#include <obs-module.h>
extern "C" {
#include "util.h"
}
#include <string>
#include "input-source.hpp"
#include "input-history.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("input-overlay", "en-US")

bool obs_module_load(void)
{
    for (int i = 0; i < MAX_SIMULTANEOUS_KEYS; i++)
        pressed_keys[i] = 0;

    register_history();
    register_overlay_source();
    start_hook();

    return true;
}

void obs_modul_unload(void)
{
    if (hook_initialized)
        end_hook();
}