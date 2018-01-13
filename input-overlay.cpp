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
//    register_history();
//    register_overlay_source();
    return true;
}
