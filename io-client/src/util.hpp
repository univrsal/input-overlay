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

#pragma once
#ifdef _WIN32
#include <cstdint>
#endif

#include <netlib.h>
#include <uiohook.h>
#include "../../io-obs/network/messages.hpp"

#ifdef _WIN32
#define STICK_MAX_VAL 32767.f
#define TRIGGER_MAX_VAL 256.f
#else
#define STICK_MAX_VAL 127.f
#define TRIGGER_MAX_VAL 256.f
#endif

#define DEBUG_LOG(fmt, ...) printf("[%25.25s:%03d]: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

namespace util {
typedef struct {
	bool monitor_gamepad;
	bool monitor_mouse;
	bool monitor_keyboard;
	char username[64];
	uint16_t port;
	ip_address ip;
} config;

extern config cfg;

enum return_codes {
	RET_NETWORK_INIT,
	RET_ARGUMENT_PARSING,
	RET_NO_HOOKS,
	RET_CONNECTION,
	RET_GAMEPAD_INIT,
	RET_UIOHOOK_INIT
};

/* Get config values and print help */
bool parse_arguments(int argc, char **args);

int send_text(char *buf);

int write_gamepad_data();

bool write_keystate(netlib_byte_buf *buffer, uint16_t code, bool pressed);

inline uint16_t swap_be16(uint16_t in)
{
	return (in >> 8) | (in << 8);
}

uint32_t get_ticks();

message recv_msg();

void close_all();

#ifdef _DEBUG
void to_bits(size_t const size, void const *const ptr);
#endif
}
