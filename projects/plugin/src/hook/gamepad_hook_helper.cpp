#include "gamepad_hook_helper.hpp"
#include <libgamepad.hpp>
#include "../util/log.h"

namespace gamepad {

std::shared_ptr<hook> hook_instance = nullptr;
bool state;

void start_pad_hook()
{
    if (state)
        return;
    hook_instance = hook::make();

    // Pipe gamepad log to obs log
    log_handler_t obs_logger;
    void *param;
    base_get_log_handler(&obs_logger, &param);
    set_logger(obs_logger, param);

    if (hook_instance->start()) {
        binfo("gamepad hook started");
        state = true;
    } else {
        bwarn("gamepad hook couldn't be started");
    }
}

void end_pad_hook()
{
    hook_instance->stop();
    state = false;
}

}