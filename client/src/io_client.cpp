/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2022 univrsal <uni@vrsal.xyz>.
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

#include "network_helper.hpp"
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

int main(int argc, char const **argv)
{
    signal(SIGINT, &sig_int__handler);
    signal(SIGBREAK, &sig_break__handler);

    if (!util::parse_arguments(argc, argv))
        return util::RET_ARGUMENT_PARSING; /* Invalid arguments */

    if (!network_helper::start())
        return util::RET_NETWORK_INIT;

    DEBUG_LOG("Network init done.");

    if (!util::cfg.monitor_keyboard && !util::cfg.monitor_mouse && !util::cfg.monitor_gamepad) {
        DEBUG_LOG("Nothing to monitor!");
        return util::RET_NO_HOOKS;
    }

    if (util::cfg.monitor_gamepad) {
        if (!gamepad_helper::start()) {
            DEBUG_LOG("Gamepad hook initialization failed!");
            return util::RET_GAMEPAD_INIT;
        }
    }

    if ((util::cfg.monitor_keyboard || util::cfg.monitor_mouse) && !uiohook_helper::start()) {
        DEBUG_LOG("uiohook init failed");
        return util::RET_UIOHOOK_INIT;
    }

    if ((!util::cfg.monitor_mouse && !util::cfg.monitor_keyboard)) {
        /* If uiohook isn't used, we need to block here until we're told to quit */
        while (network_helper::status)
            util::sleep_ms(500);
    }

    util::close_all();

    return 0;
}
