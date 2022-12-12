#pragma once
#include <uiohook.h>
#include <string>
#include <SDL2/SDL.h>

#define WSS_PAD_CONNECTED "gamepad_connected"
#define WSS_PAD_DISCONNECTED "gamepad_disconnected"
#define WSS_PAD_RECONNECTED "gamepad_reconnected"

namespace wss {
bool start();
void stop();

void dispatch_uiohook_event(const uiohook_event *, const std::string &source_name);
void dispatch_sdl_event(const SDL_Event *e, SDL_GameController *device, bool is_axis, const std::string &source_name);
}
