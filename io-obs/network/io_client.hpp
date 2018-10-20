/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <netlib.h>

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
    private:
		tcp_socket m_socket;
		uint8_t m_id;
		char* m_name;
    };
}


