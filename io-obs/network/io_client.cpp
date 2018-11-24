/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "io_client.hpp"
#include "util/element/element_button.hpp"
#include "hook/gamepad_hook.hpp"
#include "util/element/element_analog_stick.hpp"
#include "util/element/element_trigger.hpp"

namespace network
{
    io_client::io_client(char* name, tcp_socket socket, uint8_t id)
    {
        m_name = name;
        m_socket = socket;
        m_id = id;
		m_valid = true;
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

    element_data_holder* io_client::get_data()
    {
		return &m_holder;
    }

    bool io_client::read_event(netlib_byte_buf* buffer, const message msg)
    {
		auto flag = true;

        if (msg == MSG_BUTTON_DATA)
        {
            uint16_t vc = 0;
            if (!netlib_read_uint16(buffer, &vc))
                flag = false;
            m_holder.add_data(vc, element_data_button::from_buffer(buffer));
        }
        else if (msg == MSG_GAMEPAD_DATA)
        {
            uint8_t pad_id = 0, trigger_l = 0, trigger_r = 0;
            uint16_t pad_buttons = 0;
            float stick_l_x, stick_l_y, stick_r_x, stick_r_y;
            
            flag = netlib_read_uint8(buffer, &pad_id) && netlib_read_uint16(buffer, &pad_buttons) && pad_id < 4;
            blog(LOG_INFO, "Gamepad data for %i with 0x%X buttons", pad_id, pad_buttons);

            if (flag)
            {
                /* Add all buttons to the holder*/
                for (auto& btn : xinput_fix::all_codes)
                {
                    m_holder.add_gamepad_data(pad_id, xinput_fix::to_vc(btn),
                        new element_data_button((pad_buttons & btn) > 0 ? STATE_PRESSED : STATE_RELEASED));
                }

                /* Analog sticks are sent before triggers*/
                auto temp = element_data_analog_stick::from_buffer(buffer);
                if (temp)
                    temp->set_state((pad_buttons & xinput_fix::CODE_LEFT_THUMB) > 0 ? STATE_PRESSED : STATE_RELEASED,
                        (pad_buttons & xinput_fix::CODE_RIGHT_THUMB) > 0 ? STATE_PRESSED : STATE_RELEASED);
                m_holder.add_gamepad_data(pad_id, VC_STICK_DATA, temp);
                
                blog(LOG_INFO, "LEFT: %s, RIGHT: %s\n", temp->left_pressed() ? "YE" : "NE", temp->right_pressed() ? "YE" : "NE");

                m_holder.add_gamepad_data(pad_id, VC_TRIGGER_DATA,
                    element_data_trigger::from_buffer(buffer));
            }
            else
            {
                LOG_(LOG_ERROR, "Couldn't read gamepad id from buffer");
            }
        }

        if (!flag)
            LOG_(LOG_ERROR, "Couldn't read event for client %s. Error: %s", name(), netlib_get_error());
            
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
