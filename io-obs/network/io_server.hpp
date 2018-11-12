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

#define BUFFER_SIZE 5

enum message;

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
		
        void get_clients(std::vector<const char*>& v);
		
        void get_clients(obs_property_t* prop, bool enable_local);
		
        bool clients_changed() const;

		/* Checks clients and removes them
		 * if necessary
		 */
		void roundtrip(); 
		
        io_client* get_client(uint8_t id);
    private:
		bool unique_name(char* name);
        static void fix_name(char* name);
		bool create_sockets();
	
		netlib_byte_buf* m_buffer = nullptr; /* Used for temporarily storing sent data */
		bool m_clients_changed = false; /* Set to true on connection/disconnect and false after get_clients() */
		uint8_t m_num_clients;
		ip_address m_ip{};
		tcp_socket m_server;
		std::vector<std::unique_ptr<io_client>> m_clients;
    };
}


