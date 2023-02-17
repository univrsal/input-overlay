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

#define NOMINMAX /* Some windows header defines min/max */
#include "gamepad_helper.hpp"
#include "client_util.hpp"

namespace gamepad_helper {

std::mutex buffer_mutex;
buffer buf;

bool start()
{
    /* Make sure that the network is established, otherwise we might send device connections too early */
    //::util::sleep_ms(1000);
    return true;
}

void stop() {}

}
