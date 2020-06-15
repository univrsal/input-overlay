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

#include "util.hpp"
#include "gamepad.hpp"
#include "network.hpp"
#include "uiohook.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>
#define IO_CLIENT
#include "../../io-obs/util/util.hpp"

#ifdef _WIN32
#include <Windows.h>
#else
#include <cstring>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <uiohook.h>

#endif
namespace util {
config cfg;

bool parse_arguments(int argc, char **args)
{
	if (argc < 3) {
		DEBUG_LOG("io_client usage: [ip] [name] {port} {other options}\n");
		DEBUG_LOG(" [] => required {} => optional\n");
		DEBUG_LOG(" [ip]          can be ipv4 or hostname\n");
		DEBUG_LOG(" [name]        unique name to identify client (max. 64 characters)\n");
		DEBUG_LOG(" {port}        default is 1608 [1025 - %hu]\n", 0xffff);
		DEBUG_LOG(" --gamepad=1   enable/disable gamepad monitoring. Off by default\n");
		DEBUG_LOG(" --mouse=1     enable/disable mouse monitoring.  Off by default\n");
		DEBUG_LOG(" --keyboard=1  enable/disable keyboard monitoring. On by default\n");
		return false;
	}

	cfg.monitor_gamepad = false;
	cfg.monitor_keyboard = true;
	cfg.monitor_mouse = false;
	cfg.port = 1608;

	auto const s = sizeof(cfg.username);
	strncpy(cfg.username, args[2], s);
	cfg.username[s - 1] = '\0';

	if (argc > 3) {
		auto newport = uint16_t(strtol(args[3], nullptr, 0));
		if (newport > 1024) /* No system ports pls */
			cfg.port = newport;
		else
			DEBUG_LOG("%hu is outside the valid port range [1024 - %hu]\n", newport, 0xffff);
	}

	/* Resolve ip */
	if (netlib_resolve_host(&cfg.ip, args[1], cfg.port) == -1) {
		DEBUG_LOG("netlib_resolve_host failed: %s\n", netlib_get_error());
		DEBUG_LOG("Make sure obs studio is running with the remote connection enabled and configured\n");
		return false;
	}

	/* Parse additional arguments */
	std::string arg;
	for (auto i = 4; i < argc; i++) {
		arg = args[i];
		if (arg.find("--gamepad") != std::string::npos)
			cfg.monitor_gamepad = arg.find('1') != std::string::npos;
		else if (arg.find("--mouse") != std::string::npos)
			cfg.monitor_mouse = arg.find('1') != std::string::npos;
		else if (arg.find("--keyboard") != std::string::npos)
			cfg.monitor_keyboard = arg.find('1') != std::string::npos;
	}

	DEBUG_LOG("io_client configuration:\n");
	DEBUG_LOG(" Host : %s:%s\n", args[1], args[3]);
	DEBUG_LOG(" Name:     %s\n", args[2]);
	DEBUG_LOG(" Keyboard: %s\n", cfg.monitor_keyboard ? "Yes" : "No");
	DEBUG_LOG(" Mouse:    %s\n", cfg.monitor_mouse ? "Yes" : "No");
	DEBUG_LOG(" Gamepad:  %s\n", cfg.monitor_gamepad ? "Yes" : "No");

	return true;
}

/* https://www.libsdl.org/projects/SDL_net/docs/demos/tcputil.h */
int send_text(char *buf)
{
	uint32_t len, result;

	if (!buf || !strlen(buf))
		return 1;

	len = strlen(buf) + 1;
	len = netlib_swap_BE32(len);

	result = netlib_tcp_send(network::sock, &len, sizeof(len));
	if (result < sizeof(len)) {
		if (netlib_get_error() && strlen(netlib_get_error())) {
			DEBUG_LOG("netlib_tcp_send failed: %s\n", netlib_get_error());
			return 0;
		}
	}

	len = netlib_swap_BE32(len);

	result = netlib_tcp_send(network::sock, buf, len);
	if (result < len) {
		if (netlib_get_error() && strlen(netlib_get_error())) {
			DEBUG_LOG("netlib_tcp_send failed: %s\n", netlib_get_error());
			return 0;
		}
	}

	return result;
}

int write_gamepad_data()
{
	auto result = 1;
	netlib_write_uint8(network::buffer, MSG_GAMEPAD_DATA);
	for (auto &pad : gamepad::pads) {
		if (pad.changed()) {
			if (!netlib_write_uint8(network::buffer, pad.get_id()) ||
				!netlib_write_uint16(network::buffer, pad.get_state()->button_states) ||
				!netlib_write_float(network::buffer, pad.get_state()->stick_l_x) ||
				!netlib_write_float(network::buffer, pad.get_state()->stick_l_y) ||
				!netlib_write_float(network::buffer, pad.get_state()->stick_r_x) ||
				!netlib_write_float(network::buffer, pad.get_state()->stick_r_y) ||
				!netlib_write_uint8(network::buffer, pad.get_state()->trigger_l) ||
				!netlib_write_uint8(network::buffer, pad.get_state()->trigger_r))
				result = 0;

			pad.reset();
		}
	}

	if (!result)
		DEBUG_LOG("Writing event data to buffer failed: %s\n", netlib_get_error());

	return result;
}

bool write_keystate(netlib_byte_buf *buffer, uint16_t code, bool pressed)
{
	auto result = netlib_write_uint16(buffer, code);

	if (!result) {
		DEBUG_LOG("Couldn't write keycode: %s\n", netlib_get_error());
		return false;
	}

	result = netlib_write_uint8(buffer, pressed);
	if (!result) {
		DEBUG_LOG("Couldn't write keystate: %s\n", netlib_get_error());
		return false;
	}

	return true;
}

uint32_t get_ticks()
{
#ifdef _WIN32
	SYSTEMTIME time;
	GetSystemTime(&time);
	return (time.wSecond * 1000) + time.wMilliseconds;
#else
	struct timespec spec;
	clock_gettime(CLOCK_MONOTONIC, &spec);
	return round(spec.tv_nsec / 1.0e6);
#endif
}

message recv_msg()
{
	uint8_t msg_id;

	const uint32_t read_length = netlib_tcp_recv(network::sock, &msg_id, sizeof(msg_id));

	if (read_length < sizeof(msg_id)) {
		if (netlib_get_error() && strlen(netlib_get_error()))
			DEBUG_LOG("netlib_tcp_recv: %s\n", netlib_get_error());
		return MSG_READ_ERROR;
	}

	if (msg_id < MSG_LAST)
		return message(msg_id);

	DEBUG_LOG("Received message with invalid id (%i).\n", msg_id);
	return MSG_INVALID;
}

void close_all()
{
	uiohook::close();
	gamepad::end_pad_hook();
	network::close();
}

#ifdef _DEBUG
void to_bits(size_t const size, void const *const ptr)
{
	const auto b = (unsigned char *)ptr;

	for (int i = size - 1; i >= 0; i--) {
		for (auto j = 7; j >= 0; j--) {
			const unsigned char byte = (b[i] >> j) & 1;
			printf("%u", byte);
		}
	}
	puts("");
}
#endif
}
