/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
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
#include "network.hpp"
#include "client_util.hpp"
#include <messages.hpp>

namespace gamepad {

std::shared_ptr<hook> hook_instance;

bool start(uint16_t flags)
{
    /* Make sure that the network is established, otherwise we might send device connections too early */
    ::util::sleep_ms(500);
    hook_instance = hook::make(flags);
    hook_instance->set_plug_and_play(true);

    auto writer = [](const gamepad::input_event *e, uint8_t dev_idx) {
        std::lock_guard<std::mutex> lock(network::buffer_mutex);
        network::buf.write<uint8_t>(network::MSG_GAMEPAD_EVENT);
        network::buf.write<uint8_t>(dev_idx);
        network::buf.write<uint16_t>(e->vc);
        network::buf.write<float>(e->virtual_value);
        network::buf.write<uint64_t>(e->time);
    };

    hook_instance->set_axis_event_handler(
        [writer](std::shared_ptr<device> d) { writer(d->last_axis_event(), d->get_index()); });
    hook_instance->set_button_event_handler(
        [writer](std::shared_ptr<device> d) { writer(d->last_button_event(), d->get_index()); });
    hook_instance->set_connect_event_handler([](std::shared_ptr<device> d) {
        std::lock_guard<std::mutex> lock(network::buffer_mutex);
        network::buf.write<uint8_t>(network::MSG_GAMEPAD_CONNECTED);
        network::buf.write<uint8_t>(d->get_index());
        network::buf.write<uint16_t>(d->get_name().length());
        network::buf.write(d->get_name().c_str(), d->get_name().length());
    });
    return hook_instance->start();
}

void stop()
{
    if (hook_instance)
        hook_instance->stop();
}

}
