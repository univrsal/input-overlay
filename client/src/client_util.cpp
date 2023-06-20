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

#include "client_util.hpp"
#include "uiohook_helper.hpp"
#include "gamepad_helper.hpp"
#include "network_helper.hpp"
#include "argparse.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <chrono>
#include <thread>

#include <cmath>
#include <cstdio>

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#else
#include <cstring>

#include <time.h>
#include <uiohook.h>
#endif

static const char *const usages[] = {
    "basic [options] [[--] args]",
    "basic [options]",
    NULL,
};

using namespace std::chrono;

namespace util {
config cfg;

bool parse_arguments(int argc, char **args)
{
    char *user_name{};
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_STRING('a', "address", &cfg.websocket_address,
                   "websocket host address (e.g. ws://localhost:16899/)(required)", NULL, 0, 0),
        OPT_STRING('n', "name", &user_name, "name of this client (required)", NULL, 0, 0),
        OPT_BOOLEAN('k', "keyboard", &cfg.monitor_keyboard, "enable keyboard hook", NULL, 0, 0),
        OPT_BOOLEAN('m', "mouse", &cfg.monitor_mouse, "enable mouse hook", NULL, 0, 0),
        OPT_BOOLEAN('g', "gamepad", &cfg.monitor_gamepad, "enable gamepad hook", NULL, 0, 0),
        OPT_BOOLEAN('v', "verbose", &cfg.verbose, "enable verbose logging", NULL, 0, 0),
        OPT_END(),
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nClient for sending input events to the input-overlay obs plugin via websockets.",
                      "\nLicensed under the GPL 2.0");
    argparse_parse(&argparse, argc, (const char **)args);

    if (!cfg.websocket_address || strlen(cfg.websocket_address) < 3 || !user_name || strlen(user_name) < 3) {
        argparse_usage(&argparse);
        return false;
    }

    memcpy(cfg.username, user_name, std::min<size_t>(strlen(user_name), 64ul));

    binfo("io_client configuration:");
    binfo(" Host:     %s", cfg.websocket_address);
    binfo(" Name:     %s", cfg.username);
    binfo(" Keyboard: %s", cfg.monitor_keyboard ? "Yes" : "No");
    binfo(" Mouse:    %s", cfg.monitor_mouse ? "Yes" : "No");
    binfo(" Gamepad:  %s", cfg.monitor_gamepad ? "Yes" : "No");

    return true;
}

void sleep_ms(uint32_t ms)
{
    std::this_thread::sleep_for(milliseconds(ms));
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

void close_all()
{
    network_helper::stop();
    uiohook_helper::stop();
    gamepad_helper::stop();
}
}
