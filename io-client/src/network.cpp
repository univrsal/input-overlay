/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "network.hpp"
#include "hook.hpp"
#include "util.hpp"
#include <cstdio>
#include <pthread.h>

namespace network
{
	tcp_socket sock = nullptr;
	netlib_socket_set set = nullptr;
	bool network_loop = true;
    /* Shared buffer for writing data, which will be sent to the server */
	netlib_byte_buf* buffer = nullptr;

#ifdef _WIN32
	static HANDLE network_thread;
#else
	static pthread_t network_thread;
#endif

    bool start_connection(util::config* cfg)
    {
    	printf("Allocating socket...");
		set = netlib_alloc_socket_set(1);
		printf(" Done.\n");

		if (!set)
        {
			printf("netlib_alloc_socket_set failed: %s\n", netlib_get_error());
			return false;
        }

        printf("Opening socket... ");

        sock = netlib_tcp_open(&cfg->ip);

        if (!sock)
        {
			printf("netlib_tcp_open failed: %s\n", netlib_get_error());
			return false;
        }

        printf("Done.\n");

        if (netlib_tcp_add_socket(set, sock) == -1)
        {
			printf("netlib_tcp_add_socket failed: %s\n", netlib_get_error());
			return false;
        }

		printf("Connection successful!\n");
        
		/* Send client name */
		if (!util::send_text(sock, cfg->username))
        {
			printf("Failed to send username (%s): %s\n", cfg->username, netlib_get_error());
			return false;
        }

        if (!start_thread())
        {
			printf("Failed to create network thread.\n");
			return false;
        }

		return true;
    }

    bool start_thread()
    {
#ifdef _WIN32
		network_thread = CreateThread(nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(network_thread_method),
			nullptr, 0, nullptr);
		return network_thread;
#else
		return pthread_create(&network_thread, nullptr, network_thread_method, nullptr) == 0;
#endif
    }
	uint32_t last_message = 0;
#ifdef _WIN32
	DWORD WINAPI network_thread_method(const LPVOID arg)
#else
	void* network_thread_method(void *)
#endif
	{
		last_message = util::get_ticks();

        while (network_loop)
        {
            if (!listen()) /* Has a timeout of 100ms*/
            {
				printf("Received quit signal\n");
				break;
            }

            if (util::get_ticks() - last_message > DC_TIMEOUT)
            {
                /* About to timeout -> tell server we're still here */
				network::buffer->write_pos = 0;
				if (!netlib_write_uint8(network::buffer, util::MSG_PREVENT_TIMEOUT))
				{
					printf("netlib_write_uint8: %s\n", netlib_get_error());
					network_loop = false;
					break;
				}
				
				if (!netlib_tcp_send_buf(sock, buffer))
				{
					printf("netlib_tcp_send_buf: %s\n", netlib_get_error());
					network_loop = false;
					break;
				}

				last_message = util::get_ticks();
            }
        }

		hook::close();
#ifdef _WIN32
		return 0;
#else
		pthread_exit(nullptr);
#endif
	}

	int numready = 0;
    bool listen()
    {
		numready = netlib_check_socket_set(set, LISTEN_TIMEOUT);

		if (numready == -1)
		{
			printf("netlib_check_socket_set failed: %s\n", netlib_get_error());
			return false;
		}

        if (numready && netlib_socket_ready(sock))
        {
			auto msg = util::recv_msg(sock);

            switch (msg)
            {
			case util::MSG_NAME_NOT_UNIQUE:
				printf("Nickname is already in use. Disconnecting...\n");
				return false;
			case util::MSG_NAME_INVALID:
				printf("Nickname is not valid. Disconnecting...\n");
				return false;
			case util::MSG_SERVER_SHUTDOWN:
				printf("Server is shutting down.\n");
				return false;
			case util::MSG_READ_ERROR:
				printf("Couldn't read message.\n");
				return false;
			default:
			case util::MSG_INVALID:
				return false;
            }
        }
		return true;
    }

    bool init()
	{
		buffer = netlib_alloc_byte_buf(5);
        
        if (!buffer)
        {
			printf("netlib_alloc_byte_buf failed: %s\n", netlib_get_error());
			return false;
        }

		if (netlib_init() == -1)
		{
			printf("netlib_init failed: %s\n", netlib_get_error());
			return false;
		}
		return true;
    }

	void close()
	{
		if (sock)
			netlib_tcp_close(sock);
#ifdef _WIN32
		if (network_thread)
			CloseHandle(network_thread);
#endif
		if (buffer)
			netlib_free_byte_buf(buffer);
		netlib_quit();
	}
}
