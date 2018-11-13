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
    util::close_all();
}
void sig_break__handler(int signal)
{
    util::close_all();
}

int main(int argc, char **argv)
{
    signal(SIGINT, &sig_int__handler);
    signal(SIGBREAK, &sig_break__handler);

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

    /* This will block if uiohook isn't running */
    if (util::cfg.monitor_gamepad)
    {
        auto threaded = true;// util::cfg.monitor_keyboard || util::cfg.monitor_mouse;
        if (threaded)
        {
            if (!gamepad::start_pad_hook(true))
            {
                printf("Gamepad hook initialization failed!\n");
                return 5;
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
        return 6;
    }

    util::close_all();

	return 0;
}
