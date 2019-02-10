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

 /* We need 85 bytes if all four gamepads are sent + 32 bytes if all buttons are pressed down */
#define BUFFER_SIZE     118
#define LISTEN_TIMEOUT  25

namespace network
{
	extern tcp_socket sock;
	extern netlib_socket_set set;
    extern bool connected;
    extern bool state;
	extern volatile bool network_loop;
    extern volatile bool need_refresh;  /* Set to true by other threads */
    extern volatile bool data_block;    /* Set to true to prevent other threads from modifying data, which is about to be sent */
	extern netlib_byte_buf* buffer;     /* Shared buffer for writing data, which will be sent to the server */
	
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
