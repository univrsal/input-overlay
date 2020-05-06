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
#include "gamepad/gamepad_linux.hpp"
#include "hook_helper.hpp"
#include "../util/log.h"
#include "../util/obs_util.hpp"
#include <algorithm>
#include <QFile>
#include <fcntl.h>
#include <keycodes.h>
#include <unistd.h>

namespace gamepad {

static pthread_t gamepad_hook_thread;
std::mutex mutex;
bool gamepad_hook_state = false;
bool gamepad_hook_run_flag = true;
std::vector<std::shared_ptr<handle>> pads;

uint16_t last_input = VC_NONE;

bool init_pad_devices()
{
	uint8_t id = 0;
	auto flag = false;
	QString path = "/dev/input/js";

	/* Remove invalid gamepads */
	pads.erase(std::remove_if(pads.begin(), pads.end(), [&flag](handle &h) -> bool {
		if (h.valid()) {
			flag = true;
			return false;
		}
		return true;
	}));

	for (auto i = 0; i < 127; i++) { /* idk 127 gamepads seems like enough */
		if (QFile::exists(path + QString::number(i)) &&
			std::find_if(pads.begin(), pads.end(), [i](handle &h) -> bool { return h.id() == i; }) == pads.end()) {
			pads.emplace_back(std::make_shared<handle_linux>(i));
		}
	}

	return flag;
}

void start_pad_hook()
{
	if (gamepad_hook_state)
		return;

	if (init_pad_devices()) {
		gamepad_hook_state = pthread_create(&gamepad_hook_thread, nullptr, hook_method, nullptr) == 0;
	} else {
		bwarn("Gamepad device init failed\n");
	}
}

void end_pad_hook()
{
	gamepad_hook_run_flag = false;
	for (size_t i = 0; i < pads.size(); i++) {
		if (pads[i].use_count() > 1) {
			berr("Shared pointer for handle %s is still in use! (refs: %li)!", qt_to_utf8(pads[i]->get_name()),
				 pads[i].use_count());
		}
	}
}

void *hook_method(void *)
{
	while (gamepad_hook_run_flag) {
		if (!hook::input_data)
			break;
		mutex.lock();
		for (auto &pad : pads) {
			if (!pad->valid())
				continue;

			auto ptr = std::dynamic_pointer_cast<handle_linux>(pad);
			if (ptr->read_event() == -1) {
				ptr->unload();
				continue;
			}

			switch (ptr->event()->type) {
			case JS_EVENT_BUTTON:
				/* Yes, this is possible, yes it's insane
                 * but this is the first time I've ever had
                 * the chance to use it so screw it */
				if (ptr->event()->value) {
				case JS_EVENT_AXIS:
					last_input = ptr->event()->number;
				}
				//                bindings.handle_event(pad.get_id(), hook::input_data, pad.get_event());
			}
		}
		mutex.unlock();
	}
	pthread_exit(nullptr);
	return nullptr;
}

}
