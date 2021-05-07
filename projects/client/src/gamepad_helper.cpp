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
    ::util::sleep_ms(1000);
    hook_instance = hook::make(flags);
    hook_instance->set_sleep_time(mcs(600));
    hook_instance->set_plug_and_play(true, ms(1000));

    // try to load bindings, currently the file has to be provided manually
    hook_instance->load_bindings(std::string("./bindings.json"));

    auto input_writer = [](const std::shared_ptr<device> d) {
        std::lock_guard<std::mutex> lock(network::buffer_mutex);
        network::buf.write<uint8_t>(network::MSG_GAMEPAD_EVENT);
        network::buf.write<uint8_t>(d->get_index());
        network::buf.write<uint8_t>(d->get_buttons().size());
        for (const auto &btn : d->get_buttons()) {
            network::buf.write<uint16_t>(btn.first);
            network::buf.write<uint16_t>(btn.second);
        }

        network::buf.write<uint8_t>(d->get_axis().size());
        for (const auto &axis : d->get_axis()) {
            network::buf.write<uint16_t>(axis.first);
            network::buf.write<float>(axis.second);
        }

        network::buf.write<uint16_t>(d->last_axis_event()->vc);
        network::buf.write<float>(d->last_axis_event()->virtual_value);
        network::buf.write<uint64_t>(d->last_axis_event()->time);
        network::buf.write<uint16_t>(d->last_button_event()->vc);
        network::buf.write<float>(d->last_button_event()->virtual_value);
        network::buf.write<uint64_t>(d->last_button_event()->time);
    };

    auto event_writer = [](const std::shared_ptr<device> &d, network::message m) {
        std::lock_guard<std::mutex> lock(network::buffer_mutex);
        network::buf.write<uint8_t>(m);
        network::buf.write<uint8_t>(d->get_index());
        network::buf.write<uint16_t>(d->get_id().length());
        network::buf.write(d->get_id().c_str(), d->get_id().length());

        const char *state;
        switch (m) {
        case network::MSG_GAMEPAD_DISCONNECTED:
            state = "disconnected";
            break;
        case network::MSG_GAMEPAD_RECONNECTED:
            state = "reconnected";
            break;
        default:
            state = "connected";
        }
        DEBUG_LOG("Device '%s' %s\n", d->get_id().c_str(), state);
    };

    hook_instance->set_axis_event_handler(input_writer);
    hook_instance->set_button_event_handler(input_writer);
    hook_instance->set_connect_event_handler(
        [event_writer](const std::shared_ptr<device> &d) { event_writer(d, network::MSG_GAMEPAD_CONNECTED); });
    hook_instance->set_reconnect_event_handler(
        [event_writer](const std::shared_ptr<device> &d) { event_writer(d, network::MSG_GAMEPAD_RECONNECTED); });
    hook_instance->set_disconnect_event_handler(
        [event_writer](const std::shared_ptr<device> &d) { event_writer(d, network::MSG_GAMEPAD_DISCONNECTED); });
    return hook_instance->start();
}

void stop()
{
    if (hook_instance)
        hook_instance->stop();
}

}
