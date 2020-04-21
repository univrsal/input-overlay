/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <universailp@web.de>.
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

#include "gamepad_hook.hpp"
#include "hook_helper.hpp"
#include <fcntl.h>
#include <keycodes.h>
#include <unistd.h>

namespace gamepad {

static pthread_t gamepad_hook_thread;
std::mutex mutex;
bool gamepad_hook_state = false;
bool gamepad_hook_run_flag = true;
//gamepad_handle pads[PAD_COUNT];
//gamepad_binding bindings;
//uint8_t last_input = 0xff;

bool init_pad_devices()
{
    uint8_t id = 0;
    auto flag = false;
//    for (auto &pad : pads) {
//        pad.init(id++);
//        if (pad.valid())
//            flag = true;
//    }
    return flag;
}

void start_pad_hook()
{
    if (gamepad_hook_state)
        return;
    if (init_pad_devices()) {
//        gamepad_hook_state = pthread_create(&gamepad_hook_thread, NULL, hook_method, NULL) == 0;
    } else {
//        bwarn("[input-overlay] Gamepad device init failed\n");
    }
}

void end_pad_hook()
{
    gamepad_hook_run_flag = false;
}

void *hook_method(void *)
{
    while (gamepad_hook_run_flag) {
        if (!hook::input_data)
            break;
        mutex.lock();
//        for (auto &pad : pads) {
//            if (!pad.valid())
//                continue;
//            if (pad.read_event() == -1) {
//                pad.unload();
//                continue;
//            }

//            switch (pad.get_event()->type) {
//            case JS_EVENT_BUTTON:
//                /* Yes, this is possible, yes it's insane
//                 * but this is the first time I've every had
//                 * the chance to use it so screw it */
//                if (pad.get_event()->value) {
//                case JS_EVENT_AXIS:
//                    last_input = pad.get_event()->number;
//                }
//                bindings.handle_event(pad.get_id(), hook::input_data, pad.get_event());
//            }
//        }
        mutex.unlock();
    }
    pthread_exit(nullptr);
}

}
