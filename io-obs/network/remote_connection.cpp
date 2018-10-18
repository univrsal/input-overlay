/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include <obs-module.h>
#include <util/platform.h>
#include "io_server.hpp"
#include "remote_connection.hpp"

namespace network
{
    bool network_state = false;
    bool network_flag = true;
    bool log_flag = false;
	const char* local_ip = nullptr;

    io_server* server_instance = nullptr;
#ifdef _WIN32
    static HANDLE network_thread;
#else
	static pthread_t network_thread;
#endif

    const char* get_status()
    {
		return network_state ? "UP" : "DOWN";
    }

    void start_network(uint16_t port)
    {
        if (network_state)
            return;

        /* Get ip of first interface */
		ip_address addresses[2];
        if (netlib_get_local_addresses(addresses, 2) > 0)
        {
			local_ip = netlib_resolve_ip(&addresses[0]);
			if (!local_ip)
				local_ip = "localhost"; /* Better than nothing */
        }

        network_state = true;
        auto failed = false;

        if (netlib_init() == 0)
        {
            server_instance = new io_server(port);

            if (server_instance->init())
            {
                auto error = 0;
#ifdef _WIN32
                network_thread = CreateThread(nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(network_handler),
                    nullptr, 0, nullptr);
                network_state = network_thread;
                error = GetLastError();
#else
				error = pthread_create(&network_thread, nullptr, network_handler, nullptr);
                network_state = error == 0;
#endif
                if (!network_state)
                {
                    if (log_flag)
                        blog(LOG_ERROR, "[input-overlay] Server thread creation failed with code: %i", error);
                    failed = true;
                }
            }
            else
            {
                if (log_flag)
                    blog(LOG_ERROR, "[input-overlay] Server init failed");
                failed = true;
            }
        }
        else
        {
            if (log_flag)
                blog(LOG_ERROR, "[input-overlay] netlib_init failed: %s", netlib_get_error());
            failed = true;
        }

        if (failed)
        {
            if (log_flag)
                blog(LOG_ERROR, "[input-overlay] Remote connection disabled due to errors");
            close_network();
        }
    }

    void close_network()
    {
        if (network_state)
        {
            network_flag = false;
            delete server_instance;

            netlib_quit();
        }
    }

#ifdef _WIN32
    DWORD WINAPI network_handler(LPVOID arg)
#else
    void* network_handler(void*)
#endif
    {
        tcp_socket sock;

        while (network_flag)
        {
            int numready, i;
            server_instance->listen(numready);

            if (numready == -1)
            {
                if (log_flag)
                    blog(LOG_ERROR, "[input-overlay] netlib_check_sockets failed: %s", netlib_get_error());
                break;
            }

            if (!numready)
            {
                os_sleep_ms(50); /* Should be fast enough */
                continue;
            }

            if (netlib_socket_ready(server_instance->socket()))
            {
                numready--;

                if (log_flag)
                    blog(LOG_INFO, "[input-overlay] Received connection...");

                sock = netlib_tcp_accept(server_instance->socket());

                if (sock)
                {
                    char* name = nullptr;
                    if (log_flag)
                        blog(LOG_INFO, "[input-overlay] Accepted connection...");

                    if (read_text(sock, &name))
                    {
                        server_instance->add_client(sock, name);
                    }
                    else
                    {
                        if (log_flag)
                            blog(LOG_INFO, "[input-overlay] Failed to receive client name.");
                        netlib_tcp_close(sock);
                    }
                }
            }
            
            if (num_ready)
                server_instance->update_clients();
        }

#ifdef _WIN32
        return 0x0;
#else
        pthread_exit(nullptr);
#endif
    }
    
    /* https://www.libsdl.org/projects/SDL_net/docs/demos/tcputil.h */
    int send_message(tcp_socket sock, char* buf)
    {
        uint32_t length, result;
        
        if (!buf || !strlen(buf))
            return 1;
        
        len = strlen(buf) + 1;
        len = netlib_swap_BE32(len);
        
        result = netlib_tcp_send(sock, &len, sizeof(len));
        if (result < sizeof(len))
        {
            if (netlib_get_error() && strlen(netlib_get_error()))
            {
                if (log_flag)
                    blog(LOG_ERROR, "netlib_tcp_send failed: %s", netlib_get_error());
                return 0;
            }
        }
        
        len = netlib_swap_BE32(len);
        
        result = netlib_tcp_send(sock, buf, len);
        if (result < len)
        {
            if (netlib_get_error() && strlen(netlib_get_error()))
            {
                if (log_flag)
                    blog(LOG_ERROR, "netlib_tcp_send failed: %s", netlib_get_error());
                return 0;
            }
        }
        
        return result;
    }
    
    /* https://www.libsdl.org/projects/SDL_net/docs/demos/tcputil.h */
    char* read_text(tcp_socket sock, char** buf)
    {
        uint32_t len, result;

        if (*buf)
            free(*buf);
        *buf = nullptr;

        result = netlib_tcp_recv(sock, &len, sizeof(len));
        if (result < sizeof(len))
        {
            if (netlib_get_error() && strlen(netlib_get_error())) /* sometimes blank! */
                printf("netlib_tcp_recv: %s\n", netlib_get_error());
            return nullptr;
        }

        len = netlib_swap_BE32(len);
        if (!len)
            return nullptr;

        *buf = static_cast<char*>(malloc(len));
        if (!(*buf))
            return nullptr;

        result = netlib_tcp_recv(sock, *buf, len);
        if (result < len)
        {
            if (netlib_get_error() && strlen(netlib_get_error())) /* sometimes blank! */
                printf("netlib_tcp_recv: %s\n", netlib_get_error());
            free(*buf);
            buf = nullptr;
        }

        return *buf;
    }
}
