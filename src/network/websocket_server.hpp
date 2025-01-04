#pragma once
#include <uiohook.h>
#include <string>
#include <atomic>
#include "SDL.h"

#define WSS_PAD_CONNECTED "gamepad_connected"
#define WSS_PAD_DISCONNECTED "gamepad_disconnected"
#define WSS_PAD_RECONNECTED "gamepad_reconnected"

struct input_data;
namespace wss {

extern std::atomic<bool> state;
bool start();
void stop();

void dispatch_uiohook_event(const uiohook_event *, const std::string &source_name);
void dispatch_sdl_event(const SDL_Event *e, const std::string &source_name, input_data *data);
}
