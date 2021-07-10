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

#include "remote_connection.hpp"
#include "io_server.hpp"
#include "../util/config.hpp"
#include <obs-module.h>
#include <util/platform.h>
#include <thread>

#include "src/util/log.h"

#if __linux__
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
namespace network {
std::atomic<bool> network_flag;
bool local_input = false; /* True if either of the local hooks is running */
char local_ip[16] = "127.0.0.1\0";

io_server *server_instance = nullptr;
std::thread network_thread;

const char *get_status()
{
    return network_flag ? "UP" : "DOWN";
}

void start_network(uint16_t port)
{
    if (network_flag)
        return;

        /* Get ip of first interface */
#if _WIN32
    ip_address addresses[2];
    if (netlib_get_local_addresses(addresses, 2) > 0) {
        snprintf(local_ip, sizeof(local_ip), "%d.%d.%d.%d", (addresses[0].host >> 0) & 0xFF,
                 (addresses[0].host >> 8) & 0xFF, (addresses[0].host >> 16) & 0xFF, (addresses[0].host >> 24) & 0xFF);
    }
#elif __linux__
    struct ifaddrs *addrs;
    getifaddrs(&addrs);
    struct ifaddrs *tmp = addrs;
    while (tmp) {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *p_addr = (struct sockaddr_in *)tmp->ifa_addr;
            if (tmp->ifa_name != std::string("lo")) {
                snprintf(local_ip, sizeof(local_ip), "%s", inet_ntoa(p_addr->sin_addr));
                break;
            }
        }
        tmp = tmp->ifa_next;
    }
#endif
    auto failed = false;

    if (netlib_init() == 0) {
        server_instance = new io_server(port);

        if (server_instance->init()) {
            auto error = 0;
            network_flag = true;
            network_thread = std::thread(network_handler);
        } else {
            berr("Server init failed");
            failed = true;
        }
    } else {
        berr("netlib_init failed: %s", netlib_get_error());
        failed = true;
    }

    if (failed) {
        berr("Remote connection disabled due to errors");
        close_network();
    }
}

void close_network()
{
    if (network_flag) {
        network_flag = false;
        network_thread.join();
        delete server_instance;
        netlib_quit();
    }
}

void network_handler()
{
    tcp_socket sock;

    while (network_flag) {
        int numready;
        server_instance->round_trip();
        server_instance->listen(numready);

        if (numready == -1) {
            berr("netlib_check_sockets failed: %s", netlib_get_error());
            break;
        }

        if (!numready) {
            os_sleepto_ns(100000); /* Should be fast enough */
            continue;
        }

        if (netlib_socket_ready(server_instance->socket())) {
            numready--;
            binfo("Received connection...");

            sock = netlib_tcp_accept(server_instance->socket());

            if (sock) {
                char *name = nullptr;
                binfo("Accepted connection...");

                if (read_text(sock, &name)) {
                    server_instance->add_client(sock, name);
                } else {
                    berr("Failed to receive client name.");
                    netlib_tcp_close(sock);
                }
                free(name);
            }
        }

        if (numready)
            server_instance->update_clients();
    }
}

int send_message(tcp_socket sock, message msg)
{
    auto msg_id = uint8_t(msg);

    const uint32_t result = netlib_tcp_send(sock, &msg_id, sizeof(msg_id));

    if (result < sizeof(msg_id)) {
        berr("netlib_tcp_send: %s\n", netlib_get_error());
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
        berr("netlib_tcp_recv: %s\n", netlib_get_error());
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
        berr("netlib_tcp_recv: %s\n", netlib_get_error());
        free(*buf);
        buf = nullptr;
    }

    return *buf;
}

message read_msg_from_buffer(buffer &buf)
{
    auto id = buf.read<uint8_t>();

    if (id && *id <= MSG_LAST)
        return message(*id);
    return MSG_INVALID;
}
}
