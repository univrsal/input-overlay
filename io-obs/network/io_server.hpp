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

#ifdef _WIN32
#include <Windows.h>
#endif

namespace network
{
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
    private:
		bool unique_name(char* name);
		bool create_sockets();
		
		uint8_t m_num_clients;
		ip_address m_ip{};
		tcp_socket m_server;
		std::vector<std::unique_ptr<io_client>> m_clients;
    };
}


