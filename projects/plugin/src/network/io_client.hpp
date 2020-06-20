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

#pragma once

#include "../util/element/element_data_holder.hpp"
#include "remote_connection.hpp"
#include <netlib.h>

namespace network {
class io_client {
public:
	io_client(char *name, tcp_socket socket, uint8_t id);

	~io_client();

	tcp_socket socket() const;

	const char *name() const;

	uint8_t id() const;

	element_data_holder *get_data();

	bool read_event(netlib_byte_buf *buffer, message msg);

	void mark_invalid();

	bool valid() const;

private:
	element_data_holder m_holder;
	tcp_socket m_socket;
	uint8_t m_id;
	/* Set to false if this client should be disconnected on next roundtrip */
	bool m_valid;
	char *m_name;
};
}
