/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "messages.hpp"
#include <netlib.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#define TIMEOUT_NS  (1000 * 1000 * 1000)
namespace network
{
    class io_server;

    extern bool network_state; /* Initialization state*/
    extern bool network_flag; /* Running state */
    /* Set in obs_module_load */
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

    message read_msg_from_buffer(netlib_byte_buf* buf);

    int send_message(tcp_socket sock, message msg);

    extern io_server* server_instance;
}


