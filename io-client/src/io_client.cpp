/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */
#include <stdio.h>
#include <signal.h>
#include "util.hpp"
#include "network.hpp"
#include "uiohook.hpp"
#include "gamepad.hpp"

/* Catch Application closing */
void sig_int__handler(int signal)
{
    network::network_loop = false;
    gamepad::hook_run_flag = false;
}
void sig_break__handler(int signal)
{
    network::network_loop = false;
    gamepad::hook_run_flag = false;
}


int main(int argc, char **argv)
{
    signal(SIGINT, &sig_int__handler);
    signal(SIGBREAK, &sig_break__handler);
    
	if (!network::init())
		return util::RET_NETWORK_INIT;
	
    printf("Network init done.\n");

	if (!util::parse_arguments(argc, argv))
		return util::RET_ARGUMENT_PARSING; /* Invalid arguments */

    if (!util::cfg.monitor_keyboard && !util::cfg.monitor_mouse && !util::cfg.monitor_gamepad)
    {
        printf("Nothing to monitor!\n");
        return util::RET_NO_HOOKS;
    }

    printf("Arguments: Port: %hu, Name: %s, IP: %s\n", util::cfg.port, util::cfg.username, argv[1]);

	if (!network::start_connection()) /* Starts a separate network thread */
	{
		network::close();
		return util::RET_CONNECTION;
	}

    /* This will block if uiohook isn't running */
    if (util::cfg.monitor_gamepad)
    {
        auto threaded = util::cfg.monitor_keyboard || util::cfg.monitor_mouse;
        if (threaded)
        {
            if (!gamepad::start_pad_hook(true))
            {
                printf("Gamepad hook initialization failed!\n");
                return util::RET_GAMEPAD_INIT;
            }
        }
        else
        {
            gamepad::start_pad_hook(false);
        }
	}
    network::network_thread_method(nullptr);

    if ((util::cfg.monitor_keyboard || util::cfg.monitor_mouse) && !uiohook::init())
    {
        printf("uiohook init failed\n");
        return util::RET_UIOHOOK_INIT;
    }

    util::close_all();

	return 0;
}
