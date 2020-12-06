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
    hook_instance->set_sleep_time(1);

    /* Pipe gamepad log to obs log */
    auto log_pipe = [](int level, const char* msg, va_list args, void*) {
        switch (level) {
            case gamepad::LOG_DEBUG:
                blogva(LOG_DEBUG, msg, args);
            break;
            case gamepad::LOG_ERROR:
                blogva(LOG_ERROR, msg, args);
            break;
            case gamepad::LOG_INFO:
                blogva(LOG_INFO, msg, args);
            break;
            case gamepad::LOG_WARNING:
                blogva(LOG_WARNING, msg, args);
            break;
            default:;
        }
    };
    gamepad::set_logger(log_pipe, nullptr);

    hook_instance->set_axis_event_handler([](std::shared_ptr<gamepad::device> d) {
        last_input = d->last_axis_event()->native_id;
        last_input_time = d->last_axis_event()->time;
        binfo("axis: %f", d->last_axis_event()->virtual_value);
    });
    hook_instance->set_button_event_handler([](std::shared_ptr<gamepad::device> d) {
        last_input = d->last_button_event()->native_id;
        last_input_time = d->last_button_event()->time;
    });

    hook_instance->set_connect_event_handler([](std::shared_ptr<gamepad::device> d) {
        binfo("'%s' connected", d->get_name().c_str());
    });
    hook_instance->set_disconnect_event_handler([](std::shared_ptr<gamepad::device> d) {
        binfo("'%s' disconnected", d->get_name().c_str());
    });
    hook_instance->set_reconnect_event_handler([](std::shared_ptr<gamepad::device> d) {
        binfo("'%s' reconnected", d->get_name().c_str());
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
