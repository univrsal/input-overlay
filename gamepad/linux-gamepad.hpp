#ifndef LINUX_GAMEPAD_HPP
#define LINUX_GAMEPAD_HPP

#ifdef LINUX
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "../util/util.hpp"
#include "../util/layout.hpp"

#include "../hook/gamepad-hook.hpp"

struct LinuxGamepad
{
public:
	LinuxGamepad(uint8_t id, std::vector<InputKey> * keys)
	{
		m_pad_id = id;
		m_state = &pad_states[m_pad_id];
		m_keys = keys;
	}

	~LinuxGamepad()
	{
		m_keys = nullptr;
	}

	void update(uint8_t id, uint16_t r_dz, uint16_t l_dz);

    void check_keys();

	void load() { /* NO-OP */ }

	void unload() { /* NO-OP */ }

	bool is_valid() { return m_state != nullptr && m_state->valid(); }
	
	float left_stick_x() { return m_state->l_x; }
	float left_stick_y() { return m_state->l_y; }

	float right_stick_x() { return m_state->r_x; }
	float right_stick_y() { return m_state->r_y; }

	uint16_t r_dead_zone()  { return m_r_dead_zone;   }
	uint16_t l_dead_zone()  { return m_l_dead_zone;   }

	uint8_t  get_id() { return m_pad_id; }
private:
	uint8_t m_pad_id = 0;
	/* Data */
	std::vector<InputKey> * m_keys;
	uint16_t m_r_dead_zone = 0, m_l_dead_zone = 0;

    GamepadState * m_state;
};
#endif
#endif