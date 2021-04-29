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

#pragma once

#include "../util/input_data.hpp"
#include <buffer.hpp>
#include <messages.hpp>
#include <netlib.h>
#include <map>

namespace network {
class io_client {
public:
    io_client(const std::string &name, tcp_socket socket);

    ~io_client();

    tcp_socket socket() const;
    const char *name() const;
    input_data *get_data();
    bool read_event(buffer &buf, message msg);
    void mark_invalid();
    bool valid() const;

    std::map<uint8_t, std::shared_ptr<gamepad::device>> &gamepads() { return m_gamepads; }
    std::shared_ptr<gamepad::device> get_pad(const std::string &id);

private:
    bool dispatch_gamepad_input(buffer &buf);
    input_data m_holder;
    tcp_socket m_socket;
    /* Set to false if this client should be disconnected on next round_trip */
    bool m_valid;
    std::string m_name;

    /* Manually managed */
    std::map<uint8_t, std::shared_ptr<gamepad::device>> m_gamepads;
};
}
