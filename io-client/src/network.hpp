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

 /* We need 90 bytes if all four gamepads are sent + 5 bytes for a mouse event
  * (Mouse event is the largest uiohook event)
  */
#define BUFFER_SIZE     90
#define LISTEN_TIMEOUT  50
/* Can't wait exactly 1000ms because the server times clients out at 1000ms*/
#define DC_TIMEOUT      (800 - LISTEN_TIMEOUT)

namespace network
{
	extern tcp_socket sock;
	extern netlib_socket_set set;
	extern volatile bool network_loop;
    extern volatile bool data_to_send;  /* Set to true by other threads */
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
