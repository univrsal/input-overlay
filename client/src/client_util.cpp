/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2022 univrsal <uni@vrsal.xyz>.
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

#ifdef _WIN32
#include <Windows.h>
#else
#include <cstring>
#include <math.h>
#include <stdio.h>
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

bool parse_arguments(int argc, char const **args)
{
    if (argc < 3) {
        DEBUG_LOG("io_client usage: [address] [name] {other options}");
        DEBUG_LOG(" [] => required {} => optional");
        DEBUG_LOG(" [address]     websocket address of host");
        DEBUG_LOG(" [name]        unique name to identify client (max. 64 characters)");
        DEBUG_LOG(" {port}        default is 1608 [1025 - %ui]", 0xffff);
        DEBUG_LOG(" --gamepad     enable/disable gamepad monitoring. Off by default");
        DEBUG_LOG(" --mouse       enable/disable mouse monitoring.  Off by default");
        DEBUG_LOG(" --keyboard    enable/disable keyboard monitoring. On by default");
        return false;
    }

    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_STRING('a', "address", &cfg.websocke_address, "websocket host address", NULL, 0, 0),
        OPT_STRING('n', "name", &cfg.username, "name of this client", NULL, 0, 0),
        OPT_BOOLEAN('k', "keyboard", &cfg.monitor_keyboard, "enable keyboard hook (default: on)", NULL, 0, 0),
        OPT_BOOLEAN('m', "mouse", &cfg.monitor_mouse, "enable mouse hook (default: off)", NULL, 0, 0),
        OPT_BOOLEAN('g', "gamepad", &cfg.monitor_gamepad, "enable gamepad hook (default: off)", NULL, 0, 0),
        OPT_END(),
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nA brief description of what the program does and how it works.",
                      "\nAdditional description of the program after the description of the arguments.");
    argc = argparse_parse(&argparse, argc, args);

    DEBUG_LOG("io_client configuration:");
    DEBUG_LOG(" Host :    %s", cfg.websocke_address);
    DEBUG_LOG(" Name:     %s", cfg.username);
    DEBUG_LOG(" Keyboard: %s", cfg.monitor_keyboard ? "Yes" : "No");
    DEBUG_LOG(" Mouse:    %s", cfg.monitor_mouse ? "Yes" : "No");
    DEBUG_LOG(" Gamepad:  %s", cfg.monitor_gamepad ? "Yes" : "No");

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
    gamepad_helper::stop();
    network_helper::stop();
    uiohook_helper::stop();
}
}
