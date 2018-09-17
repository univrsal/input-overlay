#if 0
/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "linux_gamepad.hpp"

#ifdef LINUX

void LinuxGamepad::update(uint8_t id, uint16_t r_dz, uint16_t l_dz)
{
	m_left_state = &pad_states[id];
	m_pad_id = id;
	m_r_dead_zone = r_dz;
	m_l_dead_zone = l_dz;
}

#define SYNC_STATE(b)   ((*m_keys)[b].m_pressed = m_left_state->key_state(b))

void LinuxGamepad::check_keys()
{
	if (!m_left_state->valid())
		return;

	for (int i = 0; i < PAD_BUTTON_COUNT; i++)
		SYNC_STATE(i);
}
#endif
#endif
