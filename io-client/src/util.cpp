/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */
#include "util.hpp"
#include <cstdio>
#include <cstdlib>
#include "network.hpp"
#include <string>
#include "gamepad.hpp"
#include "hook.hpp"
#ifdef _WIN32
#include <Windows.h>
#else
#include <cstring>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <uiohook.h>

#endif
namespace util
{
	config cfg;

	bool parse_arguments(int argc, char ** args)
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

		cfg.monitor_gamepad = false;
		cfg.monitor_keyboard = true;
		cfg.monitor_mouse = false;
		cfg.port = 1608;

		auto const s = sizeof(cfg.username);
		strncpy(cfg.username, args[2], s);
		cfg.username[s - 1] = '\0';

		if (argc > 3)
		{
			auto newport = uint16_t(strtol(args[3], nullptr, 0));
			if (newport > 1024) /* No system ports pls */
				cfg.port = newport;
			else
				printf("%hu is outside the valid port range [1024 - %hu]\n", newport, 0xffff);
		}

        /* Resolve ip */
        if (netlib_resolve_host(&cfg.ip, args[1], cfg.port) == -1)
        {
			printf("netlib_resolve_host failed: %s\n", netlib_get_error());
			printf("Make sure obs studio is running with the remote connection enabled and configured\n");
			return false;
        }

        /* Parse additional arguments */
		std::string arg;
        for (auto i = 4; i < argc; i++)
        {
             arg = args[i];
             if (arg.find("--gamepad") != std::string::npos)
                 cfg.monitor_gamepad = arg.find('1') != std::string::npos;
             else if (arg.find("--mouse") != std::string::npos)
                 cfg.monitor_mouse = arg.find('1') != std::string::npos;
             else if (arg.find("--keyboard") != std::string::npos)
                 cfg.monitor_keyboard = arg.find('1') != std::string::npos;
        }

		return true;
    }

	/* https://www.libsdl.org/projects/SDL_net/docs/demos/tcputil.h */
	int send_text(char* buf)
	{
		uint32_t len, result;

		if (!buf || !strlen(buf))
			return 1;

		len = strlen(buf) + 1;
		len = netlib_swap_BE32(len);

		result = netlib_tcp_send(network::sock, &len, sizeof(len));
		if (result < sizeof(len))
		{
			if (netlib_get_error() && strlen(netlib_get_error()))
			{
				printf("netlib_tcp_send failed: %s\n", netlib_get_error());
				return 0;
			}
		}

		len = netlib_swap_BE32(len);

		result = netlib_tcp_send(network::sock, buf, len);
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

    int write_uiohook_event(uiohook_event* const event)
    {
		if (!event)
			return -1;
		auto result = 1;
		auto send = false;

		switch(event->type)
        {
		case EVENT_KEY_PRESSED:
            if (cfg.monitor_keyboard)
            {
				result = netlib_write_uint8(network::buffer, MSG_BUTTON_DATA)
					&& write_keystate(network::buffer, event->data.keyboard.keycode, true);
				send = true;
            }
			break;
		case EVENT_KEY_RELEASED:
			if (cfg.monitor_keyboard)
			{
				result = netlib_write_uint8(network::buffer, MSG_BUTTON_DATA)
					&& write_keystate(network::buffer, event->data.keyboard.keycode, false);
				send = true;
			}
			break;
        case EVENT_MOUSE_PRESSED:
			if (cfg.monitor_mouse)
			{
				result = netlib_write_uint8(network::buffer, MSG_BUTTON_DATA)
					&& write_keystate(network::buffer, event->data.mouse.button, true);
				send = true;
			}
            break;
        case EVENT_MOUSE_RELEASED:
			if (cfg.monitor_mouse)
			{
				result = netlib_write_uint8(network::buffer, MSG_BUTTON_DATA)
					&& write_keystate(network::buffer, event->data.mouse.button, false);
				send = true;
			}
            break;
        case EVENT_MOUSE_MOVED:
        case EVENT_MOUSE_DRAGGED:
			if (cfg.monitor_mouse)
			{
				result = netlib_write_uint8(network::buffer, MSG_MOUSE_POS_DATA)
					&& netlib_write_int16(network::buffer, event->data.mouse.x)
					&& netlib_write_int16(network::buffer, event->data.mouse.y);
			}
            break;
        default: ;
        }

		if (!result)
		{
            printf("Writing event data to buffer failed: %s\n", netlib_get_error());
            close_all();
		}

		return send;
    }

    int write_gamepad_data()
    {
        auto result = 1;
        netlib_write_uint8(network::buffer, MSG_GAMEPAD_DATA);
        for (auto& pad : gamepad::pad_handles)
        {
            if (pad.m_changed)
            {
                if (
                    !netlib_write_uint8(network::buffer, pad.get_id()) ||
                    !netlib_write_uint16(network::buffer, pad.get_state()->button_states) ||
                    !netlib_write_float(network::buffer, pad.get_state()->stick_l_x) ||
                    !netlib_write_float(network::buffer, pad.get_state()->stick_l_y) ||
                    !netlib_write_float(network::buffer, pad.get_state()->stick_r_x) ||
                    !netlib_write_float(network::buffer, pad.get_state()->stick_r_y) ||
                    !netlib_write_uint8(network::buffer, pad.get_state()->trigger_l) ||
                    !netlib_write_uint8(network::buffer, pad.get_state()->trigger_r))
                    result = 0;

                pad.m_changed = false;
            }
        }

        if (!result)
            printf("Writing event data to buffer failed: %s\n", netlib_get_error());
        
        return result;
    }

    int write_keystate(netlib_byte_buf* buffer, uint16_t code, bool pressed)
    {
		auto result = netlib_write_uint16(buffer, code);
		if (!result)
		{
			printf("Couldn't write keycode: %s\n", netlib_get_error());
			return -1;
		}
		
	    result = netlib_write_uint8(buffer, pressed);
        if (!result)
        {
			printf("Couldn't write keystate: %s\n", netlib_get_error());
			return -1;
        }
		return 1;
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

    message recv_msg()
    {
		uint8_t msg_id;

		const uint32_t read_length = netlib_tcp_recv(network::sock, &msg_id, sizeof(msg_id));

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

    void close_all()
    {
        hook::close();
        network::close();
        gamepad::close();
    }

#ifdef _DEBUG
    void to_bits(size_t const size, void const* const ptr)
    {
        const auto b = (unsigned char*)ptr;
        unsigned char byte;
        int i, j;

        for (i = size - 1; i >= 0; i--)
        {
            for (j = 7; j >= 0; j--)
            {
                byte = (b[i] >> j) & 1;
                printf("%u", byte);
            }
        }
        puts("");
    }
#endif
}
