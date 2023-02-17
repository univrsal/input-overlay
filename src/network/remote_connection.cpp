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

#include "remote_connection.hpp"
#include <buffer.hpp>
#include <obs-module.h>
#include <util/platform.h>
#include <thread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#if __linux__
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

namespace network {
bool local_input = false; /* True if either of the local hooks is running */
std::mutex remote_data_map_mutex;
std::unordered_map<QString, std::shared_ptr<input_data>> remote_data;

QString get_local_ip()
{
    char local_ip[64] = "127.0.0.1\0";
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
    return local_ip;
}

void process_remote_event(unsigned char *bytes, size_t len)
{
    std::lock_guard<std::mutex> lock(remote_data_map_mutex);
    buffer b(bytes, len);
    auto type = b.read<uint8_t>();
    QString client_name((QChar *)b.read(64), 64);

    auto const &data = remote_data.find(client_name);
    std::shared_ptr<input_data> client_data{};
    if (data == remote_data.end()) {
        client_data = std::make_shared<input_data>();
    } else {
        client_data = data->second;
    }

    if (type == 0) { // uiohook event
        uiohook_event *ev{};
        assert(b.data_left() >= sizeof(uiohook_event));
        ev = b.read<uiohook_event>();
        client_data->dispatch_uiohook_event(ev);
    } else { // sdl controller event
    }
}

}
