/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "uiohook.hpp"
#include "network.hpp"
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
    netlib_byte_buf* buffer = nullptr;

    volatile bool need_refresh = false;
    volatile bool data_block = false;
    volatile bool network_loop = true;
    
    bool connected = false;
    bool state = false;

#ifdef _WIN32
	static HANDLE network_thread;
#else
	static pthread_t network_thread;
#endif

    bool start_connection()
    {
    	DEBUG_LOG("Allocating socket...");
		set = netlib_alloc_socket_set(1);
		
		if (!set)
        {
			DEBUG_LOG("\nnetlib_alloc_socket_set failed: %s\n", netlib_get_error());
			return false;
        }
        printf(" Done.\n");


        DEBUG_LOG("Opening socket... ");

        sock = netlib_tcp_open(&util::cfg.ip);

        if (!sock)
        {
			DEBUG_LOG("\nnetlib_tcp_open failed: %s\n", netlib_get_error());
			return false;
        }

        printf("Done.\n");

        if (netlib_tcp_add_socket(set, sock) == -1)
        {
			DEBUG_LOG("netlib_tcp_add_socket failed: %s\n", netlib_get_error());
			return false;
        }

		DEBUG_LOG("Connection successful!\n");
        
		/* Send client name */
		if (!util::send_text(util::cfg.username))
        {
			DEBUG_LOG("Failed to send username (%s): %s\n", util::cfg.username, netlib_get_error());
			return false;
        }

        if (!start_thread())
        {
			DEBUG_LOG("Failed to create network thread.\n");
			return false;
        }
        connected = true;
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

#ifdef _WIN32
	DWORD WINAPI network_thread_method(const LPVOID arg)
#else
	void* network_thread_method(void *)
#endif
	{
        while (network_loop)
        {
            if (!listen()) /* Has a timeout of 25ms*/
            {
				DEBUG_LOG("Received quit signal\n");
                util::close_all();
				break;
            }

            if (need_refresh)
            {
                std::lock_guard<std::mutex> lock(uiohook::m_mutex);

                buffer->write_pos = 0;
                if (gamepad::check_changes() && !util::write_gamepad_data())
                {
                    DEBUG_LOG("Failed to write gamepad event data to buffer. Exiting...\n");
                    break;
                }

                if (!uiohook::data.write_to_buffer(network::buffer))
                {
                    DEBUG_LOG("Writing uiohook data to buffer failed: %s\n", netlib_get_error());
                    break;
                }

                if (!netlib_write_uint8(network::buffer, MSG_END_BUFFER))
                {
                    DEBUG_LOG("Writing buffer end failed: %s\n", netlib_get_error());
                    break;
                }

                if (buffer->write_pos > 0 && !netlib_tcp_send_buf_smart(sock, buffer))
                {
                    DEBUG_LOG("netlib_tcp_send_buf_smart: %s\n", netlib_get_error());
                    break;
                }

                
                need_refresh = false;
            }
        }

        DEBUG_LOG("Network loop exited\n");

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
				DEBUG_LOG("Nickname is already in use. Disconnecting...\n");
				return false;
			case MSG_NAME_INVALID:
				DEBUG_LOG("Nickname is not valid. Disconnecting...\n");
				return false;
			case MSG_SERVER_SHUTDOWN:
				DEBUG_LOG("Server is shutting down.\n");
				return false;
			case MSG_READ_ERROR:
				DEBUG_LOG("Couldn't read message.\n");
				return false;
            case MSG_REFRESH:
                need_refresh = true; /* fallthrough */
            case MSG_PING_CLIENT: /* NO-OP needed */
                return true;
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
        state = true;
		return true;
    }

	void close()
	{
        if (!state)
            return;
        state = false;

        /* Tell server we're disconnecting */
        if (connected)
        {
            buffer->write_pos = 0;
            netlib_write_uint8(buffer, MSG_CLIENT_DC);
            netlib_tcp_send_buf_smart(sock, buffer);
        }
        
        /* Give server time to process DC message */
#ifdef _WIN32
        Sleep(100);
#else
        usleep(100 * 1000);
#endif
        network_loop = false;

        netlib_tcp_close(sock);
        netlib_free_byte_buf(buffer);
        netlib_quit();
        buffer = NULL;
	}
}
