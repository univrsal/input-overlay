/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <netlib.h>
#include "../util/element/element_data_holder.hpp"
#include "remote_connection.hpp"

namespace network
{
	class io_client
	{
	public:
		io_client(char* name, tcp_socket socket, uint8_t id);
		~io_client();

		tcp_socket socket() const;
		const char* name() const;
		uint8_t id() const;
		element_data_holder* get_data();
		bool read_event(netlib_byte_buf* buffer, message msg);
		void mark_invalid();
		bool valid() const;
	private:
		element_data_holder m_holder;
		tcp_socket m_socket;
		uint8_t m_id;
		/* Set to false if this client should be disconnected on next roundtrip */
		bool m_valid;
		char* m_name;
    };
}


