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

#define BUFFER_SIZE     90 /* We need 89 bytes if all four gamepads are sent + 5 bytes for a mouse event*/
#define LISTEN_TIMEOUT  100
/* Can't wait exactly 1000ms because the server times clients out at 1000ms*/
#define DC_TIMEOUT      (1000 - LISTEN_TIMEOUT)

namespace network
{
	extern tcp_socket sock;
	extern netlib_socket_set set;
	extern bool network_loop;
	extern uint32_t last_message;
	extern netlib_byte_buf* network_buffer;

	bool init();
	bool start_connection();
	bool start_thread();
	bool listen();

#ifdef _WIN32
	DWORD WINAPI network_thread_method(LPVOID arg);
#else
	void* network_thread_method(void*);
#endif

	void close();
}
