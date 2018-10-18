/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <netlib.h>

#ifdef _WIN32
#include <Windows.h>
#endif

class io_server;

namespace network
{
    extern bool network_state; /* Initialization state*/
    extern bool network_flag; /* Running state */
	extern bool log_flag; /* Set in obs_module_load */
	extern const char* local_ip;

	const char* get_status();

    void start_network(uint16_t port);
    
    void close_network();
    
#ifdef _WIN32
    DWORD WINAPI network_handler(LPVOID arg);
#else
    void* network_handler(void*);
#endif

	char* read_text(tcp_socket sock, char** buf);

	int send_message(tcp_socket sock, char* buf);
	
	extern io_server* server_instance;
}


