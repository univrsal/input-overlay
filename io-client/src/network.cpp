/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "network.hpp"
#include "util.hpp"
#include <cstdio>

namespace network
{
	tcp_socket sock = nullptr;
	netlib_socket_set set = nullptr;

    bool start_connection(util::config* cfg)
    {
		set = netlib_alloc_socket_set(1);

        if (!set)
        {
			printf("netlib_alloc_socket_set failed: %s\n", netlib_get_error());
			return false;
        }
        
        sock = netlib_tcp_open(&cfg->ip);

        if (!sock)
        {
			printf("netlib_tcp_open failed: %s\n", netlib_get_error());
			return false;
        }

        if (netlib_tcp_add_socket(set, sock) == -1)
        {
			printf("netlib_tcp_add_socket failed: %s\n", netlib_get_error());
			return false;
        }

		printf("Connection successful!");
        
		/* Send client name */
		if (!util::send_message(sock, cfg->username))
        {
			printf("Failed to send username (%s): %s\n", cfg->username, netlib_get_error());
			return false;
        }

		return true;
    }

	bool init()
	{
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
		netlib_quit();
	}
}
