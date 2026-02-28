/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.xyz/alex/input-overlay
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

#include "gamepad_helper.hpp"
#include "client_util.hpp"
#include <cstdio>
#include <SDL3/SDL.h>
#include <unordered_map>

#if defined(_WIN32)
#define WIN32 1
#else
#define WIN32 0
#endif

#if defined(_WIN32)
#define WIN32 1
#define INIT_FLAGS_FOR_SDL (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD)
#else
#define INIT_FLAGS_FOR_SDL SDL_INIT_GAMEPAD
#define WIN32 0
#endif


SDL_Window *dummy_window{};

inline void sdl_init()
{
    auto link_ver = SDL_GetVersion();

    auto link_ver_major = SDL_VERSIONNUM_MAJOR(link_ver);
    auto link_ver_minor = SDL_VERSIONNUM_MINOR(link_ver);
    auto link_ver_patch = SDL_VERSIONNUM_MICRO(link_ver);

    auto compile_ver = SDL_VERSION;
    auto compile_ver_major = SDL_VERSIONNUM_MAJOR(compile_ver);
    auto compile_ver_minor = SDL_VERSIONNUM_MINOR(compile_ver);
    auto compile_ver_patch = SDL_VERSIONNUM_MICRO(compile_ver);

    binfo("Initializing SDL2 for gamepad input (compile-time: %i.%i.%i, run-time:  %i.%i.%i)", compile_ver_major,
          compile_ver_minor, compile_ver_patch, link_ver_major, link_ver_minor, link_ver_patch);

    // TODO: I think there's a flag for switching a and b on switch controllers, we might want that
    // as an option in the settings dialog
    SDL_SetHint(SDL_HINT_JOYSTICK_ENHANCED_REPORTS, "0");
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_STEAM, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_ROG_CHAKRAM, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_LINUX_DEADZONES, "1");

    if (SDL_WasInit(0) == INIT_FLAGS_FOR_SDL || !SDL_Init(INIT_FLAGS_FOR_SDL)) {
        berr("Couldn't initialize SDL: %s\n", SDL_GetError());
        return;
    }

    // I'm not sure where this file is supposed to be, but the gamepad test had this line so we'll just copy it
    SDL_AddGamepadMappingsFromFile("gamecontrollerdb.txt");

    if (WIN32) {
        dummy_window = SDL_CreateWindow("input-overlay sdl2 window", 10, 10, SDL_WINDOW_HIDDEN);
        if (!dummy_window) {
            berr("Couldn't create sdl2 window: %s\n", SDL_GetError());
            return;
        }
    }
}

inline void sdl_deinit()
{
    SDL_DestroyWindow(dummy_window);
    dummy_window = nullptr;
    SDL_Quit();
}

namespace gamepad_helper {

std::atomic<bool> state;
std::thread sdl_poll_thread;
event_queue queue;
std::unordered_map<int, SDL_Gamepad *> gamepads;

void event_loop();

bool start()
{
    if (state)
        return true;

    state = true;

    if (!WIN32)
        sdl_init();
    sdl_poll_thread = std::thread(event_loop);
    return true;
}

void stop()
{
    if (!state)
        return;
    state = false;
    sdl_poll_thread.join();
    if (!WIN32)
        sdl_deinit();
}

void event_loop()
{
    if (WIN32)
        sdl_init();
    SDL_Event event{};

    while (state) {

        /* Update to get the current event state */
        SDL_PumpEvents();

        const char *name;

        /* Process all currently pending events */
        while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_EVENT_FIRST, SDL_EVENT_LAST) == 1) {
            switch (event.type) {
                // TODO: process these events?
            case SDL_EVENT_JOYSTICK_BATTERY_UPDATED:
            case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
            case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
            case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
            case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
                break;
            case SDL_EVENT_GAMEPAD_ADDED:
                binfo("Found gamepad with id %i", event.cdevice.which);
                gamepads[event.cdevice.which] = SDL_OpenGamepad(event.cdevice.which);
                queue.mutex.lock();
                queue.events.emplace_back(event);
                queue.mutex.unlock();
                break;

            case SDL_EVENT_GAMEPAD_REMOVED:
                binfo("Gamepad with id %i disconnected", event.cdevice.which);
                SDL_CloseGamepad(gamepads[event.cdevice.which]);
                gamepads.erase(event.cdevice.which);
                queue.mutex.lock();
                queue.events.emplace_back(event);
                queue.mutex.unlock();
                break;

            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                queue.mutex.lock();
                queue.events.emplace_back(event);
                queue.mutex.unlock();
                break;
            case SDL_EVENT_QUIT:
                state = false;
                break;
            default:
                break;
            }
        }
        SDL_Delay(5); // Wait a bit to not waste performance, 5ms is arbitrary though
    }
    if (WIN32)
        sdl_deinit();
}
}
