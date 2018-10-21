/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */
#include "util.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#else
#include <math.h>
#include <stdio.h>
#include <time.h>
#endif
namespace util
{

	bool parse_arguments(int argc, char ** args, config* cfg)
	{
		if (argc < 3)
		{
			printf("io_client usage: [ip] [name] {port} {other options}\n");
			printf(" [] => required {} => optional\n");
			printf(" [ip]          can be ipv4 or hostname\n");
			printf(" [name]        unique name to identify client (max. 64 characters)\n");
			printf(" {port}        default is 1608 [1025 - %hu]\n", 0xffff);
			printf(" --gamepad=1   enable/disable gamepad monitoring. Off by default\n");
			printf(" --mouse=1     enable/disable mouse monitoring.  Off by default\n");
			printf(" --keyboard=1  enable/disable keyboard monitoring. On by default\n");
			return false;
		}

		cfg->monitor_gamepad = false;
		cfg->monitor_keyboard = true;
		cfg->monitor_mouse = false;
		cfg->port = 1608;

		auto const s = sizeof(cfg->username);
		strncpy(cfg->username, args[2], s);
		cfg->username[s - 1] = '\0';

		if (argc > 3)
		{
			auto newport = uint16_t(strtol(args[3], nullptr, 0));
			if (newport > 1024) /* No system ports pls */
				cfg->port = newport;
			else
				printf("%hu is outside the valid port range [1024 - %i]\n", newport, 0xffff);
		}

        /* Resolve ip */
        if (netlib_resolve_host(&cfg->ip, args[1], cfg->port) == -1)
        {
			printf("netlib_resolve_host failed: %s\n", netlib_get_error());
			printf("Make sure obs studio is running with the remote connection enabled and configured\n");
			return false;
        }
        
		std::string arg;
        for (auto i = 3; i < argc; i++)
        {
             arg = args[i];
             if (arg.find("--gamepad") != std::string::npos)
                 cfg->monitor_gamepad = arg.find('1') != std::string::npos;
             else if (arg.find("--mouse") != std::string::npos)
                 cfg->monitor_mouse = arg.find('1') != std::string::npos;
             else if (arg.find("--keyboard") != std::string::npos)
                 cfg->monitor_keyboard = arg.find('1') != std::string::npos;
        }

		return true;
    }

	/* https://www.libsdl.org/projects/SDL_net/docs/demos/tcputil.h */
	int send_text(tcp_socket sock, char* buf)
	{
		uint32_t len, result;

		if (!buf || !strlen(buf))
			return 1;

		len = strlen(buf) + 1;
		len = netlib_swap_BE32(len);

		result = netlib_tcp_send(sock, &len, sizeof(len));
		if (result < sizeof(len))
		{
			if (netlib_get_error() && strlen(netlib_get_error()))
			{
				printf("netlib_tcp_send failed: %s\n", netlib_get_error());
				return 0;
			}
		}

		len = netlib_swap_BE32(len);

		result = netlib_tcp_send(sock, buf, len);
		if (result < len)
		{
			if (netlib_get_error() && strlen(netlib_get_error()))
			{
			    printf("netlib_tcp_send failed: %s\n", netlib_get_error());
				return 0;
			}
		}

		return result;
	}

    int send_msg(tcp_socket sock, message msg)
    {
		auto msg_id = uint8_t(msg);

		uint32_t result = netlib_tcp_send(sock, &msg_id, sizeof(msg_id));

		if (result < sizeof(msg_id))
		{
			if (netlib_get_error() && strlen(netlib_get_error()))
				printf("netlib_tcp_recv: %s\n", netlib_get_error());
			return 0;
		}

		return result;
    }

    uint32_t get_ticks()
    {
#ifdef _WIN32
		SYSTEMTIME time;
		GetSystemTime(&time);
		return (time.wSecond * 1000) + time.wMilliseconds;
#else
		struct timespec spec;
		clock_gettime(CLOCK_MONOTONIC, &spec);
		return round(spec.tv_nsec / 1.0e6);
#endif
    }

    message recv_msg(tcp_socket sock)
    {
		uint8_t msg_id;

		const uint32_t read_length = netlib_tcp_recv(sock, &msg_id, sizeof(msg_id));

        if (read_length < sizeof(msg_id))
        {
			if (netlib_get_error() && strlen(netlib_get_error()))
				printf("netlib_tcp_recv: %s\n", netlib_get_error());
			return MSG_READ_ERROR;
        }

        switch (msg_id)
		{
		case MSG_NAME_INVALID:
			return MSG_NAME_INVALID;
		case MSG_NAME_NOT_UNIQUE:
			return MSG_NAME_NOT_UNIQUE;
		case MSG_SERVER_SHUTDOWN:
			return MSG_SERVER_SHUTDOWN;
		default:
			printf("Received message with invalid id (%i).\n", msg_id);
			return MSG_INVALID;
		}
    }
}
