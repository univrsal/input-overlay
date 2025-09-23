#include "gamepad_hook_helper.hpp"
#include "../util/log.h"
#include "../util/config.hpp"
#include "../network/websocket_server.hpp"
#include <input_data.hpp>
#include <thread>

#if defined(_WIN32)
#define WIN32 1
#define INIT_FLAGS_FOR_SDL (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD)
#else
#define INIT_FLAGS_FOR_SDL SDL_INIT_GAMEPAD
#define WIN32 0
#endif

namespace gamepad_hook {

std::atomic<bool> state;
std::thread sdl_poll_thread;
SDL_Window *dummy_window{};
gamepads *local_gamepads{};

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
    SDL_SetHint(SDL_HINT_JOYSTICK_ENHANCED_REPORTS, io_config::ds_enhanced_mode ? "1" : "0");
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_STEAM, "1");
#if SDL_VERSION_ATLEAST(2, 0, 22)
    SDL_SetHint(SDL_HINT_JOYSTICK_ROG_CHAKRAM, "1");
#endif
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
    SDL_JoystickID *ids{};
    int count{};

    ids = SDL_GetJoysticks(&count);
    
    if (count <= 0) {
        binfo("No joysticks/gamepads found");
        state = true;
        return;
    }

    for (int i = 0; i < count; ++i) {
        auto id = ids[i];
        const char *name;
        const char *path = "n/a";
        const char *description;
        auto sdl_guid = SDL_GetJoystickGUIDForID(id);
        char guid[64];

        SDL_GUIDToString(sdl_guid, guid, sizeof(guid));

        if (SDL_IsGamepad(i)) {
            char fmt[512];
            name = SDL_GetGamepadNameForID(id);
            description = controller_description(i);
            std::snprintf(fmt, 512, "%s - %s", name, description);
            add_controller(i, fmt);
        } else {
            name = SDL_GetJoystickNameForID(id);
            path = SDL_GetJoystickPathForID(id);
            description = "Joystick";
            // TODO: also register those?
        }
        bdebug("Found %s %d: %s%s%s (guid %s, VID 0x%.4x, PID 0x%.4x, player index = %d)", description, i,
               name ? name : "Unknown", path ? ", " : "", path ? path : "", guid, SDL_GetJoystickVendorForID(id),
               SDL_GetJoystickProductForID(id), SDL_GetJoystickPlayerIndexForID(id));
    }

    // Confirm object has been created
    // at end of constructor, instead
    // of during other startup/init tasks
    state = true;
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
        while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_EVENT_FIRST, SDL_EVENT_LAST) == 1) {
            if (!io_config::io_window_filters.input_blocked())
                wss::dispatch_sdl_event(&event, "local", &local_data::data);
            switch (event.type) {
            case SDL_EVENT_GAMEPAD_ADDED:
                char fmt[512];
                name = SDL_GetGamepadNameForID(event.gdevice.which);
                description = controller_description(event.gdevice.which);
                std::snprintf(fmt, 512, "%s - %s", name, description);
                add_controller(event.gdevice.which, fmt);
                binfo("Found new %s with id %i", description, event.gdevice.which);
                break;

            case SDL_EVENT_GAMEPAD_REMOVED:
                binfo("Gamepad with id %i disconnected", event.gdevice.which);
                remove_controller(event.gdevice.which);
                break;

            case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
            case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
            case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
                // TODO: process touchpad events?
                break;
            case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
                // TODO: process sensor events?
                break;

            case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
                auto pad = get_controller_from_instance_id(event.gdevice.which);
                std::lock_guard<std::mutex> lock(pad->mutex());
                pad->axis()[event.gaxis.axis] = event.gaxis.value / float(INT16_MAX);
            } break;
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            case SDL_EVENT_GAMEPAD_BUTTON_UP: {
                auto pad = get_controller_from_instance_id(event.gdevice.which);
                std::lock_guard<std::mutex> lock(pad->mutex());
                pad->buttons()[event.gbutton.button] = event.gbutton.down;
            } break;

            case SDL_EVENT_JOYSTICK_BATTERY_UPDATED:
                // TODO: process battery events?
                break;
            default:
                break;
            }
        }
        SDL_Delay(5); // Wait a bit to not waste performance, 5ms is arbitrary though
    }
}

}
