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

#include "remote_connection.hpp"
#include "io_server.hpp"
#include "../util/config.hpp"
#include "../util/obs_util.hpp"
#include <obs-module.h>
#include <string>
#include <util/platform.h>

namespace network {
bool network_state = false;
bool network_flag = false;
bool local_input = false; /* True if either of the local hooks is running */
char local_ip[16] = "127.0.0.1\0";

io_server *server_instance = nullptr;
#ifdef _WIN32
static HANDLE network_thread;
#else
static pthread_t network_thread;
#endif

const char *get_status()
{
	return network_state ? "UP" : "DOWN";
}

void start_network(uint16_t port)
{
	if (network_state)
		return;

	/* Get ip of first interface */
	ip_address addresses[2];
	if (netlib_get_local_addresses(addresses, 2) > 0) {
		snprintf(local_ip, sizeof(local_ip), "%d.%d.%d.%d", (addresses[0].host >> 0) & 0xFF,
				 (addresses[0].host >> 8) & 0xFF, (addresses[0].host >> 16) & 0xFF, (addresses[0].host >> 24) & 0xFF);
	}

	network_state = true;
	auto failed = false;

	if (netlib_init() == 0) {
		server_instance = new io_server(port);

		if (server_instance->init()) {
			auto error = 0;
			network_flag = true;
#ifdef _WIN32
			network_thread =
				CreateThread(nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(network_handler), nullptr, 0, nullptr);
			network_state = network_thread;
			error = GetLastError();
#else
			error = pthread_create(&network_thread, nullptr, network_handler, nullptr);
			network_state = error == 0;
#endif
			if (!network_state) {
				DEBUG_LOG(LOG_ERROR, "Server thread creation failed with code: %i", error);
				failed = true;
			}
		} else {
			DEBUG_LOG(LOG_ERROR, "Server init failed");
			failed = true;
		}
	} else {
		DEBUG_LOG(LOG_ERROR, "netlib_init failed: %s", netlib_get_error());
		failed = true;
	}

	if (failed) {
		DEBUG_LOG(LOG_ERROR, "Remote connection disabled due to errors");
		close_network();
	}
}

void close_network()
{
	if (network_state) {
		network_flag = false;
		delete server_instance;

		netlib_quit();
	}
}

#ifdef _WIN32
DWORD WINAPI network_handler(LPVOID arg)
#else

void *network_handler(void *)
#endif
{
	tcp_socket sock;

	while (network_flag) {
		int numready;
		server_instance->roundtrip();
		server_instance->listen(numready);

		if (numready == -1) {
			DEBUG_LOG(LOG_ERROR, "netlib_check_sockets failed: %s", netlib_get_error());
			break;
		}

		if (!numready) {
			os_sleep_ms(LISTEN_TIMEOUT); /* Should be fast enough */
			continue;
		}

		if (netlib_socket_ready(server_instance->socket())) {
			numready--;
			DEBUG_LOG(LOG_INFO, "Received connection...");

			sock = netlib_tcp_accept(server_instance->socket());

			if (sock) {
				char *name = nullptr;
				DEBUG_LOG(LOG_INFO, "Accepted connection...");

				if (read_text(sock, &name)) {
					server_instance->add_client(sock, name);
				} else {
					DEBUG_LOG(LOG_ERROR, "Failed to receive client name.");
					netlib_tcp_close(sock);
				}
			}
		}

		if (numready)
			server_instance->update_clients();
	}

#ifdef _WIN32
	return 0x0;
#else
	pthread_exit(nullptr);
#endif
}

int send_message(tcp_socket sock, message msg)
{
	auto msg_id = uint8_t(msg);

	const uint32_t result = netlib_tcp_send(sock, &msg_id, sizeof(msg_id));

	if (result < sizeof(msg_id)) {
		DEBUG_LOG(LOG_ERROR, "netlib_tcp_send: %s\n", netlib_get_error());
		return 0;
	}

	return result;
}

/* https://www.libsdl.org/projects/SDL_net/docs/demos/tcputil.h */
char *read_text(tcp_socket sock, char **buf)
{
	uint32_t len, result;

	if (*buf)
		free(*buf);
	*buf = nullptr;

	result = netlib_tcp_recv(sock, &len, sizeof(len));
	if (result < sizeof(len)) {
		DEBUG_LOG(LOG_ERROR, "netlib_tcp_recv: %s\n", netlib_get_error());
		return nullptr;
	}

	len = netlib_swap_BE32(len);
	if (!len)
		return nullptr;

	*buf = static_cast<char *>(malloc(len));
	if (!(*buf))
		return nullptr;

	result = netlib_tcp_recv(sock, *buf, len);
	if (result < len) {
		DEBUG_LOG(LOG_ERROR, "netlib_tcp_recv: %s\n", netlib_get_error());
		free(*buf);
		buf = nullptr;
	}

	return *buf;
}

message read_msg_from_buffer(netlib_byte_buf *buf)
{
	uint8_t id = 0;
	if (!netlib_read_uint8(buf, &id)) {
		DEBUG_LOG(LOG_ERROR, "netlib_read_uint8: %s\n", netlib_get_error());
		return MSG_READ_ERROR;
	}

	if (id <= MSG_LAST)
		return message(id);
	return MSG_INVALID;
}
}
