/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2021 univrsal <uni@vrsal.de>.
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
#include "gamepad_helper.hpp"
#include "network.hpp"
#include "uiohook_helper.hpp"
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

using namespace std::chrono;

namespace util {
config cfg;

bool parse_arguments(int argc, char **args)
{
    if (argc < 3) {
        DEBUG_LOG("io_client usage: [ip] [name] {port} {other options}\n");
        DEBUG_LOG(" [] => required {} => optional\n");
        DEBUG_LOG(" [ip]          can be ipv4 or hostname\n");
        DEBUG_LOG(" [name]        unique name to identify client (max. 64 characters)\n");
        DEBUG_LOG(" {port}        default is 1608 [1025 - %ui]\n", 0xffff);
        DEBUG_LOG(" --gamepad=1   enable/disable gamepad monitoring. Off by default\n");
        DEBUG_LOG(" --mouse=1     enable/disable mouse monitoring.  Off by default\n");
        DEBUG_LOG(" --keyboard=1  enable/disable keyboard monitoring. On by default\n");
        DEBUG_LOG(" --dinput      use direct input on windows. XInput is default\n");
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
            DEBUG_LOG("%hu is outside the valid port range [1024 - %ui]\n", newport, 0xffff);
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

network::message recv_msg()
{
    uint8_t msg_id;

    const uint32_t read_length = netlib_tcp_recv(network::sock, &msg_id, sizeof(msg_id));

    if (read_length < sizeof(msg_id)) {
        if (netlib_get_error() && strlen(netlib_get_error()))
            DEBUG_LOG("netlib_tcp_recv: %s\n", netlib_get_error());
        return network::MSG_READ_ERROR;
    }

    if (msg_id < network::MSG_LAST)
        return network::message(msg_id);

    DEBUG_LOG("Received message with invalid id (%i).\n", msg_id);
    return network::MSG_INVALID;
}

void close_all()
{
    uiohook::stop();
    gamepad::stop();
    network::close();
}
}
