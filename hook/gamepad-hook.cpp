/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#include "gamepad-hook.hpp"
#include <mutex>
#include <util/platform.h>
#ifdef LINUX

GamepadState pad_states[PAD_COUNT];
volatile bool gamepad_hook_state = false;

static pthread_t game_pad_hook_thread;
static std::mutex hook_mutex;

void start_pad_hook(void)
{
    init_pad_devices();

    gamepad_hook_state = true;
    int ret = pthread_create(&game_pad_hook_thread, NULL, hook_method, NULL);
    if (ret != 0) {
        blog(LOG_ERROR, "[input-overlay] Failed to create gamepad hook. Error code %i", ret);
        end_pad_hook();
    }
}

void init_pad_devices(void)
{
    hook_mutex.lock();
    pad_states[0].set_path("/dev/input/js0");
    pad_states[1].set_path("/dev/input/js1");
    pad_states[2].set_path("/dev/input/js2");
    pad_states[3].set_path("/dev/input/js3");
    hook_mutex.unlock();
}

void end_pad_hook(void)
{
    if (!gamepad_hook_state)
        return;
    hook_mutex.lock();
    pthread_cancel(game_pad_hook_thread);
    gamepad_hook_state = false;
    for (auto& pad : pad_states)
        pad.unload();
    hook_mutex.unlock();
}

#define ID_TYPE        	6
#define ID_BUTTON       1
#define ID_STATE_1      4
#define ID_STATE_2      5
#define ID_KEY_CODE     7
#define ID_PRESSED      1

#define ID_L_ANALOG_X   0
#define ID_L_ANALOG_Y   1
#define ID_L_TRIGGER    2
#define ID_R_ANALOG_X   3
#define ID_R_ANALOG_Y   4
#define ID_R_TRIGGER    5

#define STICK_MAX_VAL   127

void* hook_method(void*)
{
    while (gamepad_hook_state) {
        hook_mutex.lock();
        for (auto& pad : pad_states) {
            if (!pad.valid())
                continue;
            auto ret = pad.read_event();
            if (ret > 0 && ret < sizeof(js_event)) { /* Didn't read fully read */
                blog(LOG_ERROR, "[input-overlay] Error while reading"
                                " gamepad data from %s", pad.path().c_str());
                continue;
            } else if (ret == -1) {
                continue;
            }

            if (pad.event()->type == JS_EVENT_BUTTON) {
                pad.set_key(pad.event()->value, pad.event()->number);
            } else if (pad.event()->type == JS_EVENT_AXIS) {
                float axis = pad.event()->value / ((float)0xffff);
                switch (pad.event()->number) {
                case PAD_AXIS_LT:
                    pad.set_key(axis > 0.3f, PAD_LT);
                    break;
                case PAD_AXIS_RT:
                    pad.set_key(axis > 0.3f, PAD_RT);
                    break;
                case PAD_AXIS_LX:
                    pad.l_x = axis;
                    break;
                case PAD_AXIS_LY: /* Y axis have to be inverted */
                    pad.l_y = -axis;
                    break;
                case PAD_AXIS_RX:
                    pad.r_x = axis;
                    break;
                case PAD_AXIS_RY: /* Y axis have to be inverted */
                    pad.r_y = -axis;
                    break;
                }
            }
        }
        hook_mutex.unlock();
        os_sleep_ms(5);
    }
    return nullptr;
}

#endif
