#include "gamepad_hook_helper.hpp"
#include "../util/log.h"
#include "../util/config.hpp"
#include "../network/websocket_server.hpp"
#include <input_data.hpp>
#include <thread>

#if defined(_WIN32)
#define WIN32 1
#define INIT_FLAGS_FOR_SDL (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER)
#else
#define INIT_FLAGS_FOR_SDL (SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER)
#define WIN32 0
#endif

namespace gamepad_hook {

std::atomic<bool> state;
std::thread sdl_poll_thread;
SDL_Window *dummy_window{};
gamepads *local_gamepads{};

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

    if (SDL_WasInit(0) == INIT_FLAGS_FOR_SDL || SDL_Init(INIT_FLAGS_FOR_SDL) < 0) {
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

void start()
{
    if (state)
        return;

    /*
        So apparently sdl2 will not deliver gamepad events if the initialization
        doesn't take place in the main thread.
        On Windows sdl2 won't deliver events if initalization and event polling take place on
        differnt thread.
    */

    state = true;
    if (!WIN32) {
        sdl_init();
        local_gamepads = new gamepads;
    }

    sdl_poll_thread = std::thread([] {
        if (WIN32) {
            sdl_init();
            local_gamepads = new gamepads;
        }

        local_gamepads->event_loop();
        if (WIN32)
            sdl_deinit();
    });
}

void stop()
{
    if (!state)
        return;
    state = false;
    sdl_poll_thread.join();
    delete local_gamepads;
    local_gamepads = nullptr;
    if (!WIN32)
        sdl_deinit();
}

gamepads::gamepads()
{
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        const char *name;
        const char *path = "n/a";
        const char *description;
        char guid[64];

        SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(i), guid, sizeof(guid));

        if (SDL_IsGameController(i)) {
            char fmt[512];
            // TODO: I don't think we need more than just the name
            name = SDL_GameControllerNameForIndex(i);
            //#if SDL_VERSION_ATLEAST(2, 24, 0)
            //            path = SDL_GameControllerPathForIndex(i);
            //#endif
            description = controller_description(i);
            std::snprintf(fmt, 512, "%i %s - %s", i, name, description);
            add_controller(i, fmt);
        } else {
            name = SDL_JoystickNameForIndex(i);
#if SDL_VERSION_ATLEAST(2, 24, 0)
            path = SDL_JoystickPathForIndex(i);
#endif
            description = "Joystick";
            // TODO: also register those?
        }
        bdebug("Found %s %d: %s%s%s (guid %s, VID 0x%.4x, PID 0x%.4x, player index = %d)", description, i,
               name ? name : "Unknown", path ? ", " : "", path ? path : "", guid, SDL_JoystickGetDeviceVendor(i),
               SDL_JoystickGetDeviceProduct(i), SDL_JoystickGetDevicePlayerIndex(i));
    }
}

void gamepads::event_loop()
{
    SDL_Event event;

    while (state) {
        /* Update to get the current event state */
        SDL_PumpEvents();

        const char *name;
        const char *description;

        /* Process all currently pending events */
        while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) == 1) {
            if (!io_config::io_window_filters.input_blocked())
                wss::dispatch_sdl_event(&event, "local", &local_data::data);
            switch (event.type) {
            case SDL_CONTROLLERDEVICEADDED:
                char fmt[512];
                name = SDL_GameControllerNameForIndex(event.cdevice.which);
                description = controller_description(event.cdevice.which);
                std::snprintf(fmt, 512, "%i %s - %s", event.cdevice.which, name, description);
                add_controller(event.cdevice.which, fmt);
                binfo("Found new %s with id %i", description, event.cdevice.which);
                break;

            case SDL_CONTROLLERDEVICEREMOVED:
                binfo("Gamepad with id %i disconnected", event.cdevice.which);
                remove_controller(event.cdevice.which);
                break;

            case SDL_CONTROLLERTOUCHPADDOWN:
            case SDL_CONTROLLERTOUCHPADMOTION:
            case SDL_CONTROLLERTOUCHPADUP:
                // TODO: process touchpad events?
                break;
            case SDL_CONTROLLERSENSORUPDATE:
                // TODO: process sensor events?
                break;

            case SDL_CONTROLLERAXISMOTION: {
                auto pad = get_controller_from_instance_id(event.cdevice.which);
                std::lock_guard<std::mutex> lock(pad->mutex());
                pad->axis()[event.caxis.axis] = event.caxis.value / float(INT16_MAX);
            } break;
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP: {
                auto pad = get_controller_from_instance_id(event.cdevice.which);
                std::lock_guard<std::mutex> lock(pad->mutex());
                pad->buttons()[event.cbutton.button] = event.cbutton.state;
            } break;
#if SDL_VERSION_ATLEAST(2, 24, 0)
            case SDL_JOYBATTERYUPDATED:
                // TODO: process battery events?
                break;
#endif
            default:
                break;
            }
        }
        SDL_Delay(5); // Wait a bit to not waste performance, 5ms is arbitrary though
    }
}

}
