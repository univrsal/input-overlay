/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.xyz/alex/input-overlay
 * Copyright 2023 univrsal <uni@vrsal.xyz>.
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
#include <SDL.h>
#include <unordered_map>

#if defined(_WIN32)
#define WIN32 1
#else
#define WIN32 0
#endif

SDL_Window *dummy_window{};

inline void sdl_init()
{
    SDL_version compile_ver{}, link_ver{};

    SDL_VERSION(&compile_ver);
    SDL_GetVersion(&link_ver);

    binfo("Initializing SDL2 for gamepad input (compile-time: %i.%i.%i, run-time:  %i.%i.%i)", compile_ver.major,
          compile_ver.minor, compile_ver.patch, link_ver.major, link_ver.minor, link_ver.patch);

    // TODO: I think there's a flag for switching a and b on switch controllers, we might want that
    // as an option in the settings dialog
    SDL_SetHint(SDL_HINT_ACCELEROMETER_AS_JOYSTICK, "0");
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4_RUMBLE, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_RUMBLE, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_STEAM, "1");
#if SDL_VERSION_ATLEAST(2, 0, 22)
    SDL_SetHint(SDL_HINT_JOYSTICK_ROG_CHAKRAM, "1");
#endif
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    SDL_SetHint(SDL_HINT_LINUX_JOYSTICK_DEADZONES, "1");

    if (SDL_WasInit(0) == (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) ||
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0) {
        berr("Couldn't initialize SDL: %s\n", SDL_GetError());
        return;
    }

    // I'm not sure where this file is supposed to be, but the gamepad test had this line so we'll just copy it
    SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");

    if (WIN32) {
        dummy_window = SDL_CreateWindow("input-overlay sdl2 window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 10,
                                        10, SDL_WINDOW_HIDDEN);
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
std::unordered_map<int, SDL_GameController *> gamepads;

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
        while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) == 1) {
            switch (event.type) {
                // TODO: process these events?
#if SDL_VERSION_ATLEAST(2, 24, 0)
            case SDL_JOYBATTERYUPDATED:
#endif
            case SDL_CONTROLLERTOUCHPADDOWN:
            case SDL_CONTROLLERTOUCHPADMOTION:
            case SDL_CONTROLLERTOUCHPADUP:
            case SDL_CONTROLLERSENSORUPDATE:
                break;
            case SDL_CONTROLLERDEVICEADDED:
                binfo("Found gamepad with id %i", event.cdevice.which);
                gamepads[event.cdevice.which] = SDL_GameControllerOpen(event.cdevice.which);
                queue.mutex.lock();
                queue.events.emplace_back(event);
                queue.mutex.unlock();
                break;

            case SDL_CONTROLLERDEVICEREMOVED:
                binfo("Gamepad with id %i disconnected", event.cdevice.which);
                SDL_GameControllerClose(gamepads[event.cdevice.which]);
                gamepads.erase(event.cdevice.which);
                queue.mutex.lock();
                queue.events.emplace_back(event);
                queue.mutex.unlock();
                break;

            case SDL_CONTROLLERAXISMOTION:
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                queue.mutex.lock();
                queue.events.emplace_back(event);
                queue.mutex.unlock();
                break;
            case SDL_QUIT:
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
