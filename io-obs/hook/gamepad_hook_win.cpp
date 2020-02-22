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

#include "../util/element/element_analog_stick.hpp"
#include "../util/element/element_button.hpp"
#include "../util/element/element_data_holder.hpp"
#include "../util/element/element_dpad.hpp"
#include "../util/element/element_trigger.hpp"
#include "gamepad_hook.hpp"
#include "hook_helper.hpp"
#include <util/platform.h>

namespace gamepad {
static HANDLE hook_thread;
bool gamepad_hook_state = false;
bool gamepad_hook_run_flag = true;
gamepad_handle pads[PAD_COUNT];
std::mutex mutex;

gamepad_handle::~gamepad_handle()
{
	unload();
}

void gamepad_handle::load()
{
	unload();
	update();
	debug("Gamepad %i present: %s", m_id, valid() ? "true" : "false");
}

void gamepad_handle::update()
{
	m_valid = xinput_fix::update(m_id, &m_xin);
}

void gamepad_handle::init(uint8_t id)
{
	m_id = id;
	load();
}

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

void start_pad_hook()
{
	if (gamepad_hook_state)
		return;

	xinput_fix::load();

	if (!xinput_fix::loaded) /* Couldn't load xinput Dll*/ {
		blog(LOG_INFO, "[input-overlay] Gamepad hook init failed");
		return;
	}
	gamepad_hook_state = gamepad_hook_run_flag = init_pad_devices();

	hook_thread = CreateThread(nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(hook_method), nullptr, 0, nullptr);
	gamepad_hook_state = hook_thread;
}

void end_pad_hook()
{
	gamepad_hook_run_flag = false;
	CloseHandle(hook_thread);
}

DWORD WINAPI hook_method(const LPVOID arg)
{
	while (gamepad_hook_run_flag) {
		if (!hook::input_data)
			break;
		mutex.lock();

		for (auto &pad : pad_states) {
			if (!pad.valid())
				continue;

			dpad_direction dir[] = {dpad_direction::CENTER, dpad_direction::CENTER};

			for (const auto &button : xinput_fix::all_codes) {
				const auto state = static_cast<button_state>(pressed(pad.get_xinput(), button));
				hook::input_data->add_gamepad_data(pad.get_id(), xinput_fix::to_vc(button),
												   new element_data_button(state));
			}

			/* Dpad direction */
			get_dpad(pad.get_xinput(), dir);
			hook::input_data->add_gamepad_data(pad.get_id(), VC_DPAD_DATA, new element_data_dpad(dir[0], dir[1]));

			/* Analog sticks */
			hook::input_data->add_gamepad_data(
				pad.get_id(), VC_STICK_DATA,
				new element_data_analog_stick(pressed(pad.get_xinput(), xinput_fix::CODE_LEFT_THUMB),
											  pressed(pad.get_xinput(), xinput_fix::CODE_RIGHT_THUMB),
											  stick_l_x(pad.get_xinput()), -stick_l_y(pad.get_xinput()),
											  stick_r_x(pad.get_xinput()), -stick_r_y(pad.get_xinput())));

			/* Trigger buttons */
			hook::input_data->add_gamepad_data(pad.get_id(), VC_TRIGGER_DATA,
											   new element_data_trigger(trigger_l(pad.get_xinput()),
																		trigger_r(pad.get_xinput())));
			mutex.unlock();
		}
		os_sleep_ms(25);
	}

	return UIOHOOK_SUCCESS;
}
}
