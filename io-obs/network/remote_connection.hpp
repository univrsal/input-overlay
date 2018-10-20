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

namespace network
{
	class io_server;

    extern bool network_state; /* Initialization state*/
    extern bool network_flag; /* Running state */
	extern bool log_flag; /* Set in obs_module_load */
	extern bool local_input;
	extern char local_ip[16];

	const char* get_status();

    void start_network(uint16_t port);
    
    void close_network();
    
#ifdef _WIN32
    DWORD WINAPI network_handler(LPVOID arg);
#else
    void* network_handler(void*);
#endif

	char* read_text(tcp_socket sock, char** buf);
	
    enum message
	{
		MSG_READ_ERROR = -2,
		MSG_INVALID,
		MSG_NAME_NOT_UNIQUE,
		MSG_NAME_INVALID,
		MSG_SERVER_SHUTDOWN
	};

	int send_message(tcp_socket sock, message msg);

	extern io_server* server_instance;
}


