/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "../../sources/input_source.hpp"
#include "element_button.hpp"
#include "../../../ccl/ccl.hpp"

void data_button_from_socket(tcp_socket socket, uint16_t& vc, element_data* data)
{
	uint16_t keycode = 0;
	uint32_t result = netlib_tcp_recv(socket, &vc, sizeof(vc));
	vc = UTIL_SWAP_BE16(vc);

	if (result < sizeof(vc))
	{
		return;
	}

	uint8_t state;
	result = netlib_tcp_recv(socket, &state, sizeof(state));
	if (result < sizeof(state))
	{
		return;
	}

	data = new element_data_button(button_state(state));
}

void element_button::load(ccl_config* cfg, const std::string& id)
{
    element_texture::load(cfg, id);
    m_keycode = cfg->get_int(id + CFG_KEY_CODE);
    m_pressed = m_mapping;
    m_pressed.y = m_mapping.y + m_mapping.cy + CFG_INNER_BORDER;
    /* Checks whether first 8 bits are equal */
    is_gamepad = (m_keycode >> 8) == (VC_PAD_MASK >> 8);
}

void element_button::draw(gs_effect_t* effect, gs_image_file_t* image,
    element_data* data, sources::shared_settings* settings)
{
    if (data)
    {
        const auto button = dynamic_cast<element_data_button*>(data);
        if (button)
        {
            if (button->get_state() == STATE_PRESSED)
            {
                element_texture::draw(effect, image, &m_pressed);
            }
            else
            {
                element_texture::draw(effect, image, nullptr);
            }
        }
    }
    else
    {
        element_texture::draw(effect, image, nullptr);
    }
}
