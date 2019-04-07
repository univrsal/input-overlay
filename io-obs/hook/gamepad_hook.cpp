/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include <util/platform.h>
#include "gamepad_hook.hpp"
#include "hook_helper.hpp"
#include "../util/element/element_data_holder.hpp"
#include "../util/element/element_button.hpp"
#include "../util/element/element_analog_stick.hpp"
#include "../util/element/element_trigger.hpp"
#include "../util/element/element_dpad.hpp"

namespace gamepad
{
    bool gamepad_hook_state = false;
    bool gamepad_hook_run_flag = true;
    GamepadState pad_states[PAD_COUNT];
    std::mutex mutex;
#ifdef _WIN32
    static HANDLE hook_thread;
#else
    gamepad_binding bindings;
    uint8_t last_input = 0xff;
    static pthread_t game_pad_hook_thread;
#endif

    void start_pad_hook()
    {
        if (gamepad_hook_state)
            return;

#ifdef _WIN32
        xinput_fix::load();

        if (!xinput_fix::loaded) /* Couldn't load xinput Dll*/
        {
            blog(LOG_INFO, "[input-overlay] Gamepad hook init failed");
            return;
        }
#endif
        gamepad_hook_state = gamepad_hook_run_flag = init_pad_devices();

#ifdef _WIN32
        hook_thread = CreateThread(nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(hook_method),
            nullptr, 0, nullptr);
        gamepad_hook_state = hook_thread;
#else
        gamepad_hook_state = pthread_create(&game_pad_hook_thread, nullptr, hook_method, nullptr) == 0;
#endif
    }

    bool init_pad_devices()
    {
        uint8_t id = 0;
        auto flag = false;
        for (auto &state : pad_states) {
            state.init(id++);
            if (state.valid())
                flag = true;
        }
        return flag;
    }

    void end_pad_hook()
    {
        gamepad_hook_run_flag = false;

#ifdef _WIN32
        CloseHandle(hook_thread);
#endif
    }

    /* Background process for quering game pads */
#ifdef _WIN32
    DWORD WINAPI hook_method(const LPVOID arg)
#else

    void* hook_method(void*)
#endif
    {
        while (gamepad_hook_run_flag) {
            if (!hook::input_data)
                break;
            mutex.lock();

            for (auto &pad : pad_states) {
                if (!pad.valid())
                    continue;

#ifdef _WIN32
                dpad_direction dir[] = { DPAD_CENTER, DPAD_CENTER };

                for (const auto& button : xinput_fix::all_codes)
                {
                    const auto state = pressed(pad.get_xinput(), button);
                    hook::input_data->add_gamepad_data(pad.get_id(), xinput_fix::to_vc(button),
                        new element_data_button(state));
                }

                /* Dpad direction */
                get_dpad(pad.get_xinput(), dir);
                hook::input_data->add_gamepad_data(pad.get_id(), VC_DPAD_DATA,
                    new element_data_dpad(dir[0], dir[1]));

                /* Analog sticks */
                hook::input_data->add_gamepad_data(pad.get_id(), VC_STICK_DATA,
                    new element_data_analog_stick(
                        pressed(pad.get_xinput(), xinput_fix::CODE_LEFT_THUMB),
                        pressed(pad.get_xinput(), xinput_fix::CODE_RIGHT_THUMB),
                        stick_l_x(pad.get_xinput()), -stick_l_y(pad.get_xinput()),
                        stick_r_x(pad.get_xinput()), -stick_r_y(pad.get_xinput())
                    ));

                /* Trigger buttons */
                hook::input_data->add_gamepad_data(pad.get_id(), VC_TRIGGER_DATA,
                    new element_data_trigger(
                        trigger_l(pad.get_xinput()), trigger_r(pad.get_xinput())
                    ));
#else
                if (!pad.read_event()) {
                    pad.unload();
                    continue;
                }
                 switch(pad.get_event()->type)
                {
                    case JS_EVENT_BUTTON:
                        if (pad.get_event()->value)
                            last_input = pad.get_event()->number;
                        break;
                    case JS_EVENT_AXIS:
                        last_input = pad.get_event()->number;
                        break;
                    default: ;
                }

                bindings.handle_event(pad.get_id(), pad.get_event(), hook::input_data);

#endif /* LINUX */
                mutex.unlock();
            }
#ifdef  _WIN32 /* Delay on linux results in buffered input */
            os_sleep_ms(25);
#endif
        }

        //for (auto& state : pad_states)
        //    state.unload();
#ifdef _WIN32
        return UIOHOOK_SUCCESS;
#else
        pthread_exit(nullptr);
#endif
    }
}
