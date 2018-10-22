/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "io_client.hpp"

#include <netlib.h>
#include <vector>
#include <memory>
#include <obs-module.h>

#ifdef _WIN32
#include <Windows.h>
#endif
namespace network
{
	enum message;

    class io_server
    {
    public:
		io_server(uint16_t port);
		~io_server();

		bool init();
		void listen(int& numready);
		tcp_socket socket() const;
		void add_client(tcp_socket socket, char* name);
        void update_clients();
		void get_clients(std::vector<const char*>& v);
		void get_clients(obs_property_t* prop, bool enable_local);
		bool need_refresh() const;
		void roundtrip(); /* Checks clients for last received data */
    private:
		bool unique_name(char* name);
        static void fix_name(char* name);
		bool create_sockets();
        static message get_message_type(tcp_socket socket);
		bool receive_event(io_client* client);
		void disconnect_client(uint8_t id);

		bool m_clients_changed = false; /* Set to true on connection/disconnect and false after get_clients() */
		uint8_t m_num_clients;
		ip_address m_ip{};
		tcp_socket m_server;
		std::vector<std::unique_ptr<io_client>> m_clients;
    };
}


