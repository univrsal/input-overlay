/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.xyz/alex/input-overlay
 * Copyright 2023 univrsal <uni@vrsal.xyz>.
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

#include <cstdint>

#define blograw(level, fmt, ...) printf("%s: " fmt, level, ##__VA_ARGS__)
#define blog(level, fmt, ...) printf("%s: " fmt "\n", level, ##__VA_ARGS__)
#define bdebug(fmt, ...) blog("debug", fmt, ##__VA_ARGS__)
#define binfo(fmt, ...) blog("info", fmt, ##__VA_ARGS__)
#define berr(fmt, ...) blog("error", fmt, ##__VA_ARGS__)

namespace util {
typedef struct {
    bool verbose;
    bool monitor_gamepad;
    bool monitor_mouse;
    bool monitor_keyboard;
    char username[64];
    char *websocket_address;
} config;

extern config cfg;

enum return_codes { RET_NETWORK_INIT, RET_ARGUMENT_PARSING, RET_NO_HOOKS, RET_GAMEPAD_INIT, RET_UIOHOOK_INIT };

void sleep_ms(uint32_t ms);

/* Get config values and print help */
bool parse_arguments(int argc, char **args);

uint32_t get_ticks();

void close_all();

}
