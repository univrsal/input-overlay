/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "util.hpp"
#include "network.hpp"
#include "hook.hpp"
#include <stdio.h>
#include "gamepad.hpp"

static volatile bool quit = false;
/* Catch Application closing */
#ifdef _WIN32
#include <Windows.h>
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    gamepad::hook_run_flag = false;
    network::network_loop = false;
    hook::close();
    return TRUE;
}
#else

#endif

int main(int argc, char **argv)
{
#ifdef _WIN32
    if (!SetConsoleCtrlHandler(CtrlHandler, TRUE))
        printf("Couldn't register console event handler. Might not exit properly\n");
#else
    /* TODO: Linux console signal handling */
#endif
    
	if (!network::init())
		return 1;
	
    printf("Network init done.\n");

	if (!util::parse_arguments(argc, argv))
		return 2; /* Invalid arguments */

    if (!util::cfg.monitor_keyboard && !util::cfg.monitor_mouse && !util::cfg.monitor_gamepad)
    {
        printf("Nothing to monitor!\n");
        return 3;
    }

    printf("Arguments: Port: %hu, Name: %s, IP: %s\n", util::cfg.port, util::cfg.username, argv[1]);

	if (!network::start_connection()) /* Starts a separate network thread */
	{
		network::close();
		return 4;
	}

    if (util::cfg.monitor_gamepad)
    {
        if (!util::cfg.monitor_keyboard && !util::cfg.monitor_mouse)
        /* Start in blocking mode */
        {
            gamepad::hook_method(nullptr);
        }
        else if (!gamepad::start_pad_hook())
        {
            printf("Starting gamepad hook failed\n");
            return 5;
        }
	}

    if ((util::cfg.monitor_keyboard || util::cfg.monitor_mouse) && !hook::init())
    {
        printf("uiohook init failed\n");
        return 6;
    }

    util::close_all();
	return 0;
}
