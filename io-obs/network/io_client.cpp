/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "io_client.hpp"
#include <util/platform.h>
#include "remote_connection.hpp"

namespace network
{
    io_client::io_client(char* name, tcp_socket socket, uint8_t id)
    {
        m_name = name;
        m_socket = socket;
        m_id = id;
		m_last_message = os_gettime_ns();
    }

    io_client::~io_client()
    {
		delete m_name;
    }

    tcp_socket io_client::socket() const
    {
        return m_socket;
    }

    const char* io_client::name() const
    {
        return m_name;
    }

    uint8_t io_client::id() const
    {
		return m_id;
    }

    uint64_t io_client::last_message() const
    {
		return os_gettime_ns() - m_last_message;
    }

    void io_client::reset_timeout()
    {
		m_last_message = os_gettime_ns();
    }
}
