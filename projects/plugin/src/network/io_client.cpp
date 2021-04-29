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

#include "io_client.hpp"
#include <keycodes.h>

#include "src/util/log.h"

namespace network {
io_client::io_client(const std::string& name, tcp_socket socket) : m_holder()
{
    m_name = name;
    m_socket = socket;
    m_valid = true;
}

io_client::~io_client()
{
    netlib_tcp_close(m_socket);
}

tcp_socket io_client::socket() const
{
    return m_socket;
}

const char *io_client::name() const
{
    return m_name.c_str();
}

input_data *io_client::get_data()
{
    return &m_holder;
}

bool io_client::read_event(buffer &buf, const message msg)
{
    auto flag = true;

    if (msg == MSG_UIOHOOK_EVENT) {
        auto *event = buf.read<uiohook_event>();
        if (event) {
            m_holder.dispatch_uiohook_event(event);
        } else {
            flag = false;
        }
    } else if (msg == MSG_GAMEPAD_EVENT) {
    }

    if (!flag)
        berr("Couldn't read event for client %s. Error: %s", name(), netlib_get_error());

    return flag;
}

bool io_client::valid() const
{
    return m_valid;
}

void io_client::mark_invalid()
{
    m_valid = false;
}
}
