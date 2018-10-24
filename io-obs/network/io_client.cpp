/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "io_client.hpp"
#include <util/platform.h>
#include "util/element/element_button.hpp"

namespace network
{
    io_client::io_client(char* name, tcp_socket socket, uint8_t id)
    {
        m_name = name;
        m_socket = socket;
        m_id = id;
		m_valid = true;
		m_last_message = os_gettime_ns();
    }

    io_client::~io_client()
    {
		delete m_name;
		netlib_tcp_close(m_socket);
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

    element_data_holder* io_client::get_data()
    {
		return &m_holder;
    }

    bool io_client::read_event(netlib_byte_buf* buffer, const message msg)
    {
		element_data* data = nullptr;
		uint16_t vc = 0;
		uint8_t pad_id = 0;
		auto flag = true;

		if (!netlib_read_uint16(buffer, &vc))
			flag = false;

        switch (msg)
        {
		case MSG_BUTTON_DATA:
			data = element_data_button::from_buffer(buffer);
			break;
		default:;
        }

        if (!flag || !data)
        {
            LOG_(LOG_ERROR, "Couldn't read event for client %s. Error: %s", name(), netlib_get_error());
        }
        else
        {
            m_holder.add_data(vc, data);
        }
            
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
