/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.cc/alex/input-overlay
 * Copyright 2025 univrsal <uni@vrsal.xyz>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#pragma once
#include <uiohook.h>
#include <string>
#include <atomic>
#include <SDL3/SDL.h>

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
