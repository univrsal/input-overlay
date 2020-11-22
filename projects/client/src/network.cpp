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

#include "network.hpp"
#include "gamepad_helper.hpp"
#include "uiohook_helper.hpp"
#include "client_util.hpp"
#include <cstdio>

namespace network {
tcp_socket sock = nullptr;
netlib_socket_set set = nullptr;
buffer buf;
volatile bool need_refresh = false;
volatile bool data_block = false;
volatile bool network_loop = true;

bool connected = false;
bool state = false;

std::thread network_thread;
std::mutex buffer_mutex;

bool start_connection()
{
	DEBUG_LOG("Allocating socket...");
	set = netlib_alloc_socket_set(1);

	if (!set) {
		DEBUG_LOG("\nnetlib_alloc_socket_set failed: %s\n", netlib_get_error());
		return false;
	}
	printf(" Done.\n");

	DEBUG_LOG("Opening socket... ");

	sock = netlib_tcp_open(&util::cfg.ip);

	if (!sock) {
		DEBUG_LOG("\nnetlib_tcp_open failed: %s\n", netlib_get_error());
		return false;
	}

	printf("Done.\n");

	if (netlib_tcp_add_socket(set, sock) == -1) {
		DEBUG_LOG("netlib_tcp_add_socket failed: %s\n", netlib_get_error());
		return false;
	}

	DEBUG_LOG("Connection successful!\n");

	/* Send client name */
	if (!util::send_text(util::cfg.username)) {
		DEBUG_LOG("Failed to send username (%s): %s\n", util::cfg.username, netlib_get_error());
		return false;
	}

	start_thread();
	connected = true;
	return true;
}

void start_thread()
{
	network_thread = std::thread(network_thread_method);
}

void network_thread_method()
{
	while (network_loop) {
		if (!listen()) /* Has a timeout of 25ms*/
		{
			DEBUG_LOG("Received quit signal\n");
			util::close_all();
			break;
		}

		std::lock_guard<std::mutex> lock(buffer_mutex);
		/* Reset scroll wheel if no scroll event happened for a bit */
		if (util::get_ticks() - uiohook::last_scroll_time >= SCROLL_TIMEOUT) {
			buf.write<uint8_t>(network::MSG_BUTTON_DATA);
			buf.write<uint16_t>(VC_MOUSE_WHEEL);
			buf.write<uint8_t>(0);
			buf.write<uint16_t>(0);
			buf.write<uint64_t>(util::get_ticks());
		}

		/* Send any data written to the buffer */
		if (buf.write_pos() > 0) {
			if (!netlib_tcp_send(sock, buf.get(), buf.write_pos())) {
				DEBUG_LOG("netlib_tcp_send: %s\n", netlib_get_error());
				break;
			}
			buf.reset();
		}
	}

	DEBUG_LOG("Network loop exited\n");
}

int numready = 0;
bool listen()
{
	numready = netlib_check_socket_set(set, LISTEN_TIMEOUT);

	if (numready == -1) {
		DEBUG_LOG("netlib_check_socket_set failed: %s\n", netlib_get_error());
		return false;
	}

	if (numready && netlib_socket_ready(sock)) {
		auto msg = util::recv_msg();

		switch (msg) {
		case MSG_NAME_NOT_UNIQUE:
			DEBUG_LOG("Nickname is already in use. Disconnecting...\n");
			return false;
		case MSG_NAME_INVALID:
			DEBUG_LOG("Nickname is not valid. Disconnecting...\n");
			return false;
		case MSG_SERVER_SHUTDOWN:
			DEBUG_LOG("Server is shutting down.\n");
			return false;
		case MSG_READ_ERROR:
			DEBUG_LOG("Couldn't read message.\n");
			return false;
		case MSG_REFRESH:
			need_refresh = true; /* fallthrough */
		case MSG_PING_CLIENT:    /* NO-OP needed */
			return true;
		default:
		case MSG_INVALID:
			return false;
		}
	}
	return true;
}

bool init()
{
	if (netlib_init() == -1) {
		DEBUG_LOG("netlib_init failed: %s\n", netlib_get_error());
		return false;
	}
	state = true;
	return true;
}

void close()
{
	if (!state)
		return;
	state = false;

	/* Tell server we're disconnecting */
	if (connected) {
		buf.reset();
		buf.write<uint8_t>(MSG_CLIENT_DC);
		netlib_tcp_send(sock, buf.get(), buf.write_pos());
	}

	/* Give server time to process DC message */
	util::sleep_ms(100);
	network_loop = false;
	network_thread.join();
	netlib_tcp_close(sock);
	netlib_quit();
}
}
