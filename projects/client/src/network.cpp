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

#include "network.hpp"
#include "uiohook_helper.hpp"
#include "client_util.hpp"
#include <cstdio>

namespace network {
tcp_socket sock = nullptr;
netlib_socket_set set = nullptr;
buffer buf;
std::atomic<bool> network_loop;
bool connected = false;

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
    network_loop = true;
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
            network_loop = false; // The rest will be taken care of in the main thread
            break;
        }

        std::lock_guard<std::mutex> lock(buffer_mutex);
        /* Reset scroll wheel if no scroll event happened for a bit */
        if (uiohook::last_scroll_time > 0 && util::get_ticks() - uiohook::last_scroll_time >= SCROLL_TIMEOUT) {
            buf.write<uint8_t>(MSG_MOUSE_WHEEL_RESET);
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
    if (util::cfg.monitor_keyboard || util::cfg.monitor_mouse) {
        // We have to stop the hook here since the main thread is blocked by uiohook
        uiohook::stop();
    }
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
        case MSG_REFRESH:;       /* fallthrough */
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
    return true;
}

void close()
{
    if (!network_loop)
        return;
    network_loop = false;

    /* Tell server we're disconnecting */
    if (connected) {
        buf.reset();
        buf.write<uint8_t>(MSG_CLIENT_DC);
        netlib_tcp_send(sock, buf.get(), buf.write_pos());
    }

    /* Give server time to process DC message */
    util::sleep_ms(100);
    network_thread.join();
    netlib_tcp_close(sock);
    netlib_quit();
}
}
