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
#include "gamepad.hpp"
#ifdef UNIX
#include <pthread.h>
#endif

namespace network
{
	tcp_socket sock = nullptr;
	netlib_socket_set set = nullptr;
	bool network_loop = true;
    netlib_byte_buf* buffer = nullptr;

    volatile bool data_to_send = false;
    volatile bool data_block = false;

#ifdef _WIN32
	static HANDLE network_thread;
#else
	static pthread_t network_thread;
#endif

    bool start_connection()
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

        sock = netlib_tcp_open(&util::cfg.ip);

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
		if (!util::send_text(util::cfg.username))
        {
			printf("Failed to send username (%s): %s\n", util::cfg.username, netlib_get_error());
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

            /* If any data is ready send it */
            if (data_to_send)
            {
                data_block = true;
                
                /* First write pending data */
                if (gamepad::check_changes() && !util::write_gamepad_data())
                {
                    printf("Failed to write gamepad event data to buffer. Exiting...\n");
                    break;
                }

#ifdef _DEBUG /* Visualize gamepad data */
                util::to_bits(network::buffer->write_pos, network::buffer);
#endif
                if (hook::new_event)
                    util::write_uiohook_event(hook::last_event);
                
                if (!netlib_tcp_send_buf_smart(sock, buffer))
                {
                    DEBUG_LOG("netlib_tcp_send_buf_smart: %s\n", netlib_get_error());
                    //break;
                }

                data_block = false;
                data_to_send = false;
                hook::new_event = false;
                //last_message = util::get_ticks();
                buffer->write_pos = 0;
            }
            /* About to timeout -> tell server we're still here */
            else if (util::get_ticks() - last_message > DC_TIMEOUT)
            {
                buffer->write_pos = 0;

				if (!netlib_write_uint8(buffer, MSG_PREVENT_TIMEOUT))
				{
					DEBUG_LOG("netlib_write_uint8: %s\n", netlib_get_error());
					break;
				}

				if (!netlib_tcp_send_buf_smart(sock, buffer))
				{
                    DEBUG_LOG("netlib_tcp_send_buf_smart: %s\n", netlib_get_error());
					break;
				}
                printf("yezsz\n");
				last_message = util::get_ticks();
            }
        }

        network_loop = false;
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
            DEBUG_LOG("netlib_check_socket_set failed: %s\n", netlib_get_error());
			return false;
		}

        if (numready && netlib_socket_ready(sock))
        {
			auto msg = util::recv_msg();

            switch (msg)
            {
			case MSG_NAME_NOT_UNIQUE:
				printf("Nickname is already in use. Disconnecting...\n");
				return false;
			case MSG_NAME_INVALID:
				printf("Nickname is not valid. Disconnecting...\n");
				return false;
			case MSG_SERVER_SHUTDOWN:
				printf("Server is shutting down.\n");
				return false;
			case MSG_READ_ERROR:
				printf("Couldn't read message.\n");
				return false;
			default:
			case MSG_INVALID:
				return false;
            }
        }
		return true;
    }

    bool init()
	{
		if (netlib_init() == -1)
		{
            DEBUG_LOG("netlib_init failed: %s\n", netlib_get_error());
			return false;
		}

		buffer = netlib_alloc_byte_buf(BUFFER_SIZE);

        if (!buffer)
        {
            DEBUG_LOG("netlib_alloc_byte_buf failed: %s\n", netlib_get_error());
			return false;
        }
		return true;
    }

	void close()
	{
		if (sock)
			netlib_tcp_close(sock);
#ifdef _WIN32
		if (network_loop && network_thread)
		{
            CloseHandle(network_thread);
            network_thread = nullptr;
		}
#endif  
			
		if (buffer)
			netlib_free_byte_buf(buffer);
        
		netlib_quit();
	}
}
