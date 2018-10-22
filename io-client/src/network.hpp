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
#include "util.hpp"

#define LISTEN_TIMEOUT  100
/* Can't wait exactly 500ms because the server times clients out 500ms*/
#define DC_TIMEOUT      (400)

namespace network
{
	extern tcp_socket sock;
	extern netlib_socket_set set;
	extern bool network_loop;
	extern uint32_t last_message;
	bool init();
	bool start_connection(util::config* cfg);
	bool start_thread();
	bool listen();

#ifdef _WIN32
	DWORD WINAPI network_thread_method(LPVOID arg);
#else
	void* network_thread_method(void*);
#endif

	void close();
}