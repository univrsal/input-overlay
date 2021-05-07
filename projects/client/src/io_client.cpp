/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2021 univrsal <uni@vrsal.de>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#include "network.hpp"
#include "uiohook_helper.hpp"
#include "gamepad_helper.hpp"
#include "client_util.hpp"
#include <signal.h>
#include <stdio.h>

#ifndef SIGBREAK
#define SIGBREAK SIGQUIT /* SIGBREAK only exists on Windows, so just use quit otherwise */
#endif

/* Catch Application closing */
void sig_int__handler(int)
{
    util::close_all();
}

void sig_break__handler(int)
{
    util::close_all();
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

    if (!util::cfg.monitor_keyboard && !util::cfg.monitor_mouse && !util::cfg.monitor_gamepad) {
        printf("Nothing to monitor!\n");
        return util::RET_NO_HOOKS;
    }

    if (!network::start_connection()) /* Starts a separate network thread */
    {
        network::close();
        return util::RET_CONNECTION;
    }

    if (util::cfg.monitor_gamepad) {
        if (!libgamepad::start(util::cfg.gamepad_hook_type)) {
            printf("Gamepad hook initialization failed!\n");
            return util::RET_GAMEPAD_INIT;
        }
    }

    if ((util::cfg.monitor_keyboard || util::cfg.monitor_mouse) && !uiohook::start()) {
        printf("uiohook init failed\n");
        return util::RET_UIOHOOK_INIT;
    }

    if ((!util::cfg.monitor_mouse && !util::cfg.monitor_keyboard)) {
        /* If uiohook isn't used, we need to block here until we're told to quit */
        while (network::network_loop)
            util::sleep_ms(500);
    }

    util::close_all();

    return 0;
}
