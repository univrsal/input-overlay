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

#include "gamepad_helper.hpp"
#include "network.hpp"
#include "client_util.hpp"
#include <messages.hpp>
namespace gamepad {

std::shared_ptr<hook> hook_instance;

bool start(hook_type t)
{
	hook_instance = hook::make(t);
	return hook_instance->start();
}

void stop()
{
	if (hook_instance)
		hook_instance->stop();
}

bool write_data()
{
	static uint64_t last_event = 0;
	uint64_t n = last_event;
	auto result = false;
	netlib_write_uint8(network::buffer, network::MSG_GAMEPAD_DATA);

	hook_instance->get_mutex()->lock();
	for (auto &dev : gamepad::hook_instance->get_devices()) {
		uint64_t e = std::max(dev->last_axis_event()->time, dev->last_button_event()->time);
		if (e > last_event) {
			n = std::max(n, e);

			result |= netlib_write_uint8(network::buffer, dev->get_index());
			result |= netlib_write_uint16(network::buffer, dev->last_axis_event()->vc);
			result |= netlib_write_float(network::buffer, dev->last_axis_event()->virtual_value);
			result |= netlib_write_uint16(network::buffer, dev->last_button_event()->vc);
			result |= netlib_write_float(network::buffer, dev->last_button_event()->virtual_value);
		}

		if (netlib_tcp_send_buf_smart(network::sock, network::buffer) != network::buffer->write_pos) {
			result = 0;
		}
	}
	hook_instance->get_mutex()->unlock();
	last_event = n;

	if (!result)
		DEBUG_LOG("Writing event data to buffer failed: %s\n", netlib_get_error());

	return result;
}

}
