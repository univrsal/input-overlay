/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#include "io_server.hpp"
#include "remote_connection.hpp"
#include "util/util.hpp"
#include "util/config.hpp"
#include <obs-module.h>
#include <util/platform.h>
#include <algorithm>

static netlib_socket_set sockets = nullptr;

namespace network
{
    std::mutex mutex;

    io_server::io_server(const uint16_t port) : m_server(nullptr)
    {
        sockets = nullptr;
        m_num_clients = 0;
        m_ip.port = port;
        m_last_refresh = os_gettime_ns();
    }

    io_server::~io_server()
    {
        /* Smart pointer will delete 
         * and destructor will close socket
         */
        m_clients.clear();
    }

    bool io_server::init()
    {
        auto flag = true;

        if (netlib_resolve_host(&m_ip, nullptr, m_ip.port) == -1) {
            DEBUG_LOG(LOG_ERROR, "netlib_resolve_host failed: %s.", netlib_get_error());
            flag = false;
        } else {
            const auto ipaddr = netlib_swap_BE32(m_ip.host);
            DEBUG_LOG(LOG_INFO, "Remote connection open on %d.%d.%d.%d:%hu", ipaddr >> 24, ipaddr >> 16 & 0xff,
                 ipaddr >> 8 & 0xff, ipaddr & 0xff, m_ip.port);

            m_server = netlib_tcp_open(&m_ip);
            m_buffer = netlib_alloc_byte_buf(BUFFER_SIZE);

            if (!m_buffer)
                DEBUG_LOG(LOG_ERROR, "netlib_alloc_byte_buf failed; %s", netlib_get_error());

            if (!m_server) {
                DEBUG_LOG(LOG_ERROR, "netlib_tcp_open failed: %s", netlib_get_error());
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
        mutex.lock();

        for (const auto &client : m_clients) {
            if (netlib_socket_ready(client->socket())) {
                /* Receive input data */
                m_buffer->read_pos = 0; /* Reset buffer */
                const int read = netlib_tcp_recv_buf(client->socket(), m_buffer);

                if (read < 0) {
                    DEBUG_LOG(LOG_ERROR, "Failed to receive buffer from %s. Closed connection", client->name());
                    client->mark_invalid();
                    continue;
                }

                while (m_buffer->read_pos < read) /* Buffer can contain multiple messages */
                {
                    const auto msg = read_msg_from_buffer(m_buffer);

                    switch (msg) {
                        case MSG_MOUSE_DATA:
                        case MSG_BUTTON_DATA:
                        case MSG_GAMEPAD_DATA:
                            if (!client->read_event(m_buffer, msg))
                                DEBUG_LOG(LOG_ERROR, "Failed to receive event data from %s.", client->name());
                            break;
                        case MSG_CLIENT_DC:
                            client->mark_invalid();
                            break;
                        default:
                        case MSG_END_BUFFER:
                        case MSG_INVALID:
                            break;
                    }
                }
            }
        }
        mutex.unlock();
    }

    void io_server::get_clients(std::vector<const char*> &v)
    {
        for (const auto &client : m_clients) {
            v.emplace_back(client->name());
        }
        m_clients_changed = false;
    }

    void io_server::get_clients(obs_property_t* prop, const bool enable_local)
    {
        obs_property_list_clear(prop);

        if (enable_local)
            obs_property_list_add_int(prop, T_LOCAL_SOURCE, 0);

        for (const auto &client : m_clients) {
            obs_property_list_add_int(prop, client->name(), client->id() + 1); /* 0 is for local input */
        }
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
            const auto old = server_instance->m_num_clients;
            m_clients.erase(std::remove_if(m_clients.begin(), m_clients.end(), [](const std::unique_ptr<io_client> &o)
            {
                if (!o->valid()) {
                    server_instance->m_num_clients--;
                    DEBUG_LOG(LOG_INFO, "%s disconnected.", o->name());
                    return true;
                }
                return false;
            }), m_clients.end());

            if ((os_gettime_ns() - m_last_refresh) / (1000 * 1000) > io_config::refresh_rate) {
                for (auto &client : m_clients) {
                    if (!send_message(client->socket(), MSG_REFRESH))
                        client->mark_invalid();
                }
                m_last_refresh = os_gettime_ns();
            }

            if (old != server_instance->m_num_clients)
                m_clients_changed = true;
        }

        mutex.unlock();
    }

    io_client* io_server::get_client(const uint8_t id)
    {
        if (id < m_clients.size())
            return m_clients[id].get();
        return nullptr;
    }

    void io_server::add_client(tcp_socket socket, char* name)
    {
        std::lock_guard<std::mutex> lock(mutex);

        fix_name(name);

        if (!strlen(name)) {
            DEBUG_LOG(LOG_INFO, "Disconnected %s: Invalid name", name);
            send_message(socket, MSG_NAME_INVALID);
            netlib_tcp_close(socket);
            return;
        }

        if (!unique_name(name)) {
            DEBUG_LOG(LOG_INFO, "Disconnected %s: Name already in use", name);
            send_message(socket, MSG_NAME_NOT_UNIQUE);
            netlib_tcp_close(socket);
            return;
        }

        DEBUG_LOG(LOG_INFO, "Received connection from '%s'.", name);

        m_clients_changed = true;
        m_clients.emplace_back(new io_client(name, socket, m_num_clients));
        m_num_clients++;
    }

    bool io_server::unique_name(char* name)
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
    void io_server::fix_name(char* name)
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

        sockets = netlib_alloc_socket_set(m_num_clients + 1);
        if (!sockets) {
            DEBUG_LOG(LOG_ERROR, "netlib_alloc_socket_set failed with %i clients.", m_num_clients + 1);
            network_flag = false;
            return false;
        }

        netlib_tcp_add_socket(sockets, m_server);

        for (const auto &client : m_clients)
            netlib_tcp_add_socket(sockets, client->socket());

        return true;
    }
}
