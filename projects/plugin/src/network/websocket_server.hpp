#pragma once
#include <uiohook.h>
#include <string>
#include <libgamepad.hpp>

#define WSS_PAD_CONNECTED "gamepad_connected"
#define WSS_PAD_DISCONNECTED "gamepad_disconnected"
#define WSS_PAD_RECONNECTED "gamepad_reconnected"

namespace wss {
bool start();
void stop();

void dispatch_uiohook_event(const uiohook_event *, const std::string &source_name);
void dispatch_gamepad_event(const gamepad::input_event *e, const std::shared_ptr<gamepad::device> &device, bool is_axis,
                            const std::string &source_name);
void dispatch_gamepad_event(const std::shared_ptr<gamepad::device> &device, const char *state,
                            const std::string &source_name);
}