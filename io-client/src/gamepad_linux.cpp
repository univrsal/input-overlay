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

#include "gamepad.hpp"
#include "network.hpp"
#include <fcntl.h>
#include <stdio.h>
#include <thread>
#include <unistd.h>

namespace gamepad {
volatile bool state = false;
volatile bool run_flag = true;
gamepad_handle pads[PAD_COUNT];
pthread_t thread;

gamepad_handle::~gamepad_handle()
{
	unload();
}

void gamepad_handle::update()
{
	/* NO-OP */
}

void gamepad_handle::unload()
{
	close(m_device_id);
}

void gamepad_handle::load()
{
	m_device_id = open(m_path.c_str(), O_RDONLY);
	m_valid = m_device_id >= 0;
	DEBUG_LOG("Gamepad %i present: %s", m_id, valid() ? "true" : "false");
}

void gamepad_handle::init(const uint8_t id)
{
	unload();
	m_id = id;
	m_path.clear();
	m_path = "/dev/input/js";
	m_path.append(std::to_string(m_id));
	load();
}

js_event *gamepad_handle::get_event()
{
	return &m_event;
}

int gamepad_handle::read_event()
{
	ssize_t bytes;
	bytes = read(m_device_id, &m_event, sizeof(m_event));
	if (bytes == sizeof(m_event))
		return 0;

	/* Error, could not read full event. */
	return -1;
}

/* Hook util methods */

bool init_pad_devices()
{
	uint8_t id = 0;
	auto flag = false;
	for (auto &pad : pads) {
		pad.init(id++);
		if (pad.valid())
			flag = true;
	}
	return flag;
}

bool start_pad_hook(bool threaded)
{
	state = run_flag = init_pad_devices();
	if (!state) {
		DEBUG_LOG("Initializing gamepads failed\n");
		return false;
	}

	if (threaded) {
		state = pthread_create(&thread, nullptr, hook_method, nullptr) == 0;
	} else {
		state = true;
		hook_method(nullptr);
	}
	return state;
}

bool init_pads()
{
	uint8_t id = 0;
	auto flag = false;
	for (auto &state : pads) {
		state.init(id++);
		if (state.valid())
			flag = true;
	}
	return flag;
}

void close()
{
	if (!state)
		return;
	state = false;
	run_flag = false;
}

bool check_changes()
{
	if (!util::cfg.monitor_gamepad)
		return false;
	//    for (auto &pad : pad_handles)
	//        if (pad.m_changed)
	//            return true;
	return false;
}

/* Background process for querying game pads */
void *hook_method(void *)
{
	/* The hook only keeps track of the gamepad states here
     * and the changes will then be sent in a different thread
     */
	while (run_flag) {
		for (auto &pad : pads) {
			if (!pad.valid())
				continue;
			if (pad.read_event() == -1) {
				pad.unload();
				continue;
			}

			switch (pad.get_event()->type) {
			case JS_EVENT_BUTTON:
				/* Yes, this is possible, yes it's insane
                     * but this is the first time I've every had
                     * the chance to use it so screw it */
				if (pad.get_event()->value) {
				case JS_EVENT_AXIS:;
				}
			}
		}
	}

	for (auto &pad : pads)
		pad.unload();

	pthread_exit(nullptr);
}
} /* namespace gamepad */
