/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
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

#include <mutex>


namespace gamepad {

void start_pad_hook();
void end_pad_hook();
bool init_pad_devices();
void *hook_method(void *);

/* Mutex for thread safety */
extern std::mutex mutex;
/* Four structs containing info to query gamepads */
//extern std::vector<std::shared_ptr<handle>> pads;
/* Init state of hook */
extern bool gamepad_hook_state;
/* False will end thread */
extern bool gamepad_hook_run_flag;

}
