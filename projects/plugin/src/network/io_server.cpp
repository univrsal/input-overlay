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

#include "io_server.hpp"
#include "remote_connection.hpp"
#include "../util/config.hpp"
#include "../util/lang.h"
#include <algorithm>
#include <obs-module.h>
#include <util/platform.h>

#include "src/util/log.h"

static netlib_socket_set sockets = nullptr;

namespace network {
std::mutex mutex;

io_server::io_server(const uint16_t port) : m_server(nullptr)
{
    sockets = nullptr;
    m_ip.port = port;
    m_last_refresh = os_gettime_ns();
}

io_server::~io_server()
{
    m_clients.clear();
}

bool io_server::init()
{
    auto flag = true;

    if (netlib_resolve_host(&m_ip, nullptr, m_ip.port) == -1) {
        berr("netlib_resolve_host failed: %s.", netlib_get_error());
        flag = false;
    } else {
        const auto ipaddr = netlib_swap_BE32(m_ip.host);
        binfo("Remote connection open on %d.%d.%d.%d:%hu", ipaddr >> 24, ipaddr >> 16 & 0xff, ipaddr >> 8 & 0xff,
             ipaddr & 0xff, m_ip.port);

        m_server = netlib_tcp_open(&m_ip);
        m_buffer.resize(8192); // Most likely will never need more than 8KB
        if (!m_server) {
            berr("netlib_tcp_open failed: %s", netlib_get_error());
            flag = false;
        }
    }
    return flag;
}

void io_server::listen(int &numready)
{
    if (create_sockets())
        numready = netlib_check_socket_set(sockets, LISTEN_TIMEOUT);
}

tcp_socket io_server::socket() const
{
    return m_server;
}

void io_server::update_clients()
{
    std::lock_guard<std::mutex> lock(mutex);

    for (const auto &client : m_clients) {
        if (netlib_socket_ready(client->socket())) {
            /* Receive input data */
            m_buffer.reset();
            const int read = netlib_tcp_recv(client->socket(), static_cast<void *>(m_buffer.get()), m_buffer.length());

            if (read < 0) {
                berr("Failed to receive buffer from %s. Closed connection", client->name());
                client->mark_invalid();
                continue;
            }

            while (m_buffer.read_pos() < read) /* Buffer can contain multiple messages */
            {
                const auto msg = read_msg_from_buffer(m_buffer);

                switch (msg) {
                case MSG_UIOHOOK_EVENT:
                case MSG_GAMEPAD_EVENT:
                    if (!client->read_event(m_buffer, msg))
                        berr("Failed to receive event data from %s.", client->name());
                    break;
                case MSG_MOUSE_WHEEL_RESET:
                    client->get_data()->last_wheel_event = {};
                    break;
                case MSG_CLIENT_DC:
                    client->mark_invalid();
                    break;
                case MSG_GAMEPAD_CONNECTED:
                    break;
                default:
                case MSG_END_BUFFER:
                case MSG_INVALID:
                    break;
                }
            }
        }
    }
}

void io_server::get_clients(std::vector<const char *> &v)
{
    for (const auto &client : m_clients) {
        v.emplace_back(client->name());
    }
    m_clients_changed = false;
}

void io_server::get_clients(obs_property_t *prop, const bool enable_local)
{
    obs_property_list_clear(prop);

    if (enable_local)
        obs_property_list_add_string(prop, T_LOCAL_SOURCE, "");

    for (const auto &client : m_clients)
        obs_property_list_add_string(prop, client->name(), client->name());
}

bool io_server::clients_changed() const
{
    return m_clients_changed;
}

void io_server::ping_clients()
{
    for (auto &client : m_clients) {
        if (!send_message(client->socket(), MSG_PING_CLIENT))
            client->mark_invalid(); /* Can't send data -> Connection is dead */
    }
}

void io_server::roundtrip()
{
    mutex.lock();

    if (!m_clients.empty()) {
        const auto old = server_instance->num_clients();
        const auto it = std::remove_if(m_clients.begin(), m_clients.end(), [](const std::unique_ptr<io_client> &o) {
            if (!o->valid()) {
                binfo("%s disconnected.", o->name());
                return true;
            }
            return false;
        });
        m_clients.erase(it, m_clients.end());

        if ((os_gettime_ns() - m_last_refresh) / (1000 * 1000) > io_config::server_refresh_rate) {
            for (auto &client : m_clients) {
                if (!send_message(client->socket(), MSG_REFRESH))
                    client->mark_invalid();
            }
            m_last_refresh = os_gettime_ns();
        }

        if (old != server_instance->num_clients())
            m_clients_changed = true;
    }

    mutex.unlock();
}

io_client *io_server::get_client(const std::string& id)
{
    for (auto& client : m_clients) {
        if (client->name() == id)
            return client.get();
    }
    return nullptr;
}

void io_server::add_client(tcp_socket socket, char *name)
{
    std::lock_guard<std::mutex> lock(mutex);

    fix_name(name);

    if (!strlen(name)) {
        binfo("Disconnected %s: Invalid name", name);
        send_message(socket, MSG_NAME_INVALID);
        netlib_tcp_close(socket);
        return;
    }

    if (!unique_name(name)) {
        binfo("Disconnected %s: Name already in use", name);
        send_message(socket, MSG_NAME_NOT_UNIQUE);
        netlib_tcp_close(socket);
        return;
    }

    bdebug("Received connection from '%s'.", name);

    m_clients_changed = true;
    m_clients.emplace_back(new io_client(name, socket));
}

bool io_server::unique_name(char *name)
{
    if (!name)
        return false;
    auto flag = true;
    for (auto &client : m_clients) {
        if (!strcmp(name, client->name())) {
            flag = false;
            break;
        }
    }
    return flag;
}

/* Only works with pre-allocated char arrays */
void io_server::fix_name(char *name)
{
    static auto accepted = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-0123456789";
    size_t pos;
    const auto len = strlen(name);
    while ((pos = strspn(name, accepted)) != len) {
        name[pos] = '_';
    }
}

bool io_server::create_sockets()
{
    if (sockets)
        netlib_free_socket_set(sockets);

    sockets = netlib_alloc_socket_set(num_clients() + 1);
    if (!sockets) {
        berr("netlib_alloc_socket_set failed with %i clients.", num_clients() + 1);
        network_flag = false;
        return false;
    }

    netlib_tcp_add_socket(sockets, m_server);

    for (const auto &client : m_clients)
        netlib_tcp_add_socket(sockets, client->socket());

    return true;
}
}
