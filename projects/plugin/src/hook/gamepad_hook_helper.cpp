#include "gamepad_hook_helper.hpp"
#include <libgamepad.hpp>
#include "../util/log.h"
#include "../util/config.hpp"

namespace libgamepad {

std::shared_ptr<gamepad::hook> hook_instance = nullptr;
bool state;
uint16_t last_input;
uint64_t last_input_time;

void start_pad_hook()
{
    if (state)
        return;
    hook_instance = gamepad::hook::make(io_config::use_dinput ? gamepad::hook_type::DIRECT_INPUT
                                                              : gamepad::hook_type::NATIVE_DEFAULT);
    hook_instance->set_plug_and_play(true);
    hook_instance->set_sleep_time(10);

    /* Pipe gamepad log to obs log */
    log_handler_t obs_logger;
    void *param;
    base_get_log_handler(&obs_logger, &param);
    gamepad::set_logger(obs_logger, param);

    hook_instance->set_axis_event_handler([](std::shared_ptr<gamepad::device> d) {
        last_input = d->last_axis_event()->native_id;
        last_input_time = d->last_axis_event()->time;
    });
    hook_instance->set_button_event_handler([](std::shared_ptr<gamepad::device> d) {
        last_input = d->last_button_event()->native_id;
        last_input_time = d->last_button_event()->time;
    });

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
