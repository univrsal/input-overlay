/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "io_server.hpp"
#include <obs-module.h>
#include <QStringList>
#include "remote_connection.hpp"
#include "util/util.hpp"

static netlib_socket_set sockets = nullptr;

namespace network
{
    io_server::io_server(const uint16_t port)
        : m_server(nullptr)
    {
        sockets = nullptr;
        m_num_clients = 0;
        m_ip.port = port;
    }

    io_server::~io_server()
    {
        m_clients.clear();
    }

    bool io_server::init()
    {
		auto flag = true;

        if (netlib_resolve_host(&m_ip, nullptr, m_ip.port) == -1)
        {
            if (log_flag)
                blog(LOG_ERROR, "[input-overlay] netlib_resolve_host failed: %s. ", netlib_get_error());
			flag = false;
        }
        else
        {
            if (log_flag)
            {
                const auto ipaddr = netlib_swap_BE32(m_ip.host);
                blog(LOG_INFO, "[input-overlay] Remote connection opened on %d.%d.%d.%d:%hu",
                    ipaddr >> 24, ipaddr >> 16 & 0xff, ipaddr >> 8 & 0xff, ipaddr & 0xff, m_ip.port);
            }

			m_server = netlib_tcp_open(&m_ip);

			if (!m_server)
			{
				blog(LOG_ERROR, "[input-overlay] netlib_tcp_open failed: %s", netlib_get_error());
				flag = false;
			}
		}
		return flag;
    }

    void io_server::listen(int& numready)
    {
        if (create_sockets())
            numready = netlib_check_socket_set(sockets, (uint32_t)-1); /* Wrapping intentional */
    }

    tcp_socket io_server::socket() const
    {
        return m_server;
    }
    
    void io_server::update_clients()
    {
		uint8_t id = 0;
        for (auto& const client : m_clients)
        {
            if (netlib_socket_ready(client->socket()))
            {
                /* Receive input data */
                if (receive_event(client->socket()))
                {
                    
                }
                else
                {
                    disconnect_client(id);
                }
            }
			id++;
        }
    }

    void io_server::get_clients(QStringList& list)
    {

		for (const auto& client : m_clients)
		{
			list.append(client->name());
		}
		m_clients_changed = false;
    }

    void io_server::get_clients(obs_property_t* prop, const bool enable_local)
    {
		auto i = 1; /* 0 is reserved for local connection */
		
        obs_property_list_clear(prop);

		if (enable_local)
			obs_property_list_add_int(prop, T_LOCAL_SOURCE, 0);

		for (const auto& client : m_clients)
		{
			obs_property_list_add_int(prop, client->name(), i++);
		}
    }

    bool io_server::need_refresh() const
    {
		return m_clients_changed;
    }

    void io_server::add_client(tcp_socket socket, char* name)
    {
		fix_name(name);

        if (!strlen(name))
        {
            if (log_flag)
                blog(LOG_INFO, "Disconnected %s: Invalid name", name);
			send_message(socket, MSG_NAME_INVALID);
            netlib_tcp_close(socket);
            return;
        }

        if (!unique_name(name))
        {
            if (log_flag)
                blog(LOG_INFO, "Disconnected %s: Name already in use", name);
			send_message(socket, MSG_NAME_NOT_UNIQUE);
            netlib_tcp_close(socket);
            return;
        }

		if (log_flag)
			blog(LOG_INFO, "[input-overlay] Received connection from '%s'.", name);

		m_clients_changed = true;
        m_clients.emplace_back(new io_client(name, socket, m_num_clients));
        m_num_clients++;
    }

    bool io_server::unique_name(char* name)
    {
        if (!name)
            return false;
        auto flag = true;
        for (auto& client : m_clients)
        {
            if (!strcmp(name, client->name()))
            {
                flag = false;
                break;
            }
        }
        return flag;
    }

    /* Only works with prealloccated char arrays */
    void io_server::fix_name(char* name)
    {
		static const char* accepted = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-0123456789";
		size_t pos;
        const auto len = strlen(name);
		while ((pos = strspn(name, accepted)) != len)
		{
			name[pos] = '_';
		}
    }

    bool io_server::create_sockets()
    {
        int i;
        if (sockets)
            netlib_free_socket_set(sockets);

        sockets = netlib_alloc_socket_set(m_num_clients + 1);
        if (!sockets)
        {
            if (log_flag)
            {
                blog(LOG_ERROR,
                    "[input-overlay] netlib_alloc_socket_set failed with %i clients.",
                    m_num_clients + 1);
            }

            network_flag = false;
            return false;
        }

        netlib_tcp_add_socket(sockets, m_server);

        for (const auto& client : m_clients)
            netlib_tcp_add_socket(sockets, client->socket());

        return true;
    }

    /* Reads libuihook event or gamepad event from socket*/
    bool io_server::receive_event(tcp_socket socket)
    {
		return true;
    }

    void io_server::disconnect_client(const uint8_t index)
	{
		netlib_tcp_close(m_clients[index]->socket());
		m_num_clients--;
		m_clients.erase(m_clients.begin() + index);
    }
}
