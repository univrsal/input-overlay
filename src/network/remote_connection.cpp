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
#include "websocket_server.hpp"
#include "../util/config.hpp"
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
std::unordered_map<std::string, std::shared_ptr<input_data>> remote_data;

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

void process_remote_event(struct mg_connection *ws, unsigned char *bytes, size_t len)
{
    buffer b(bytes, len);
    auto *name = (char *)b.read(64);
    if (!name)
        return;
    std::string client_name = name;
    input_data *client_data{};

    // To prevent a map lookup for every event
    // we save the input_data pointer in fn_data
    // the shared pointer has a longer lifetime than the websocket connection
    // so this should be fine, if we clean up remote_data at some point
    // we'd have to make sure that the websocket doesn't exist anymore
    if (ws->fn_data) {
        client_data = static_cast<input_data *>(ws->fn_data);
    } else {
        std::lock_guard<std::mutex> lock(remote_data_map_mutex);
        auto const &data = remote_data.find(client_name);
        if (data == remote_data.end()) {
            auto new_data = std::make_shared<input_data>();
            remote_data[client_name] = new_data;
            client_data = new_data.get();
        } else {
            client_data = data->second.get();
        }
        ws->fn_data = (void *)client_data;
    }
    auto const blocked = io_config::io_window_filters.input_blocked();

    while (b.data_left() > 0) {
        auto type = b.read<uint8_t>();
        if (*type == 0) { // uiohook event
            uiohook_event *ev{};
            ev = b.read<uiohook_event>();
            if (!ev)
                break;
            std::lock_guard<std::mutex> lock(client_data->m_mutex);
            client_data->dispatch_uiohook_event(ev);

            if (!blocked)
                wss::dispatch_uiohook_event(ev, client_name);
        } else { // sdl controller event
        }
    }
}

}
