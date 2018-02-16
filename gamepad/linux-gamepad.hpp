#ifndef LINUX_GAMEPAD_HPP
#define LINUX_GAMEPAD_HPP

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "../util/util.hpp"
#include "../util/layout.hpp"

struct LinuxGamepad
{
public:
	LinuxGamepad(std::string path, std::vector<InputKey> * keys)
	{
		m_device_path = path;
		m_keys = keys;
	}

	~LinuxGamepad()
	{
		unload();
		m_keys = nullptr;
	}

	void load();
	void unload();
	void update(std::string path, uint16_t r_dz, uint16_t l_dz);

	void check_keys();

	bool is_valid() { return m_file != nullptr; }
	
	float left_stick_x() { return m_l_stick_x; }
	float left_stick_y() { return m_l_stick_y; }

	float right_stick_x() { return m_r_stick_x; }
	float right_stick_y() { return m_r_stick_y; }

	uint16_t r_dead_zone()  { return m_r_dead_zone;   }
	uint16_t l_dead_zone()  { return m_l_dead_zone;   }

private:
	void add_key(uint8_t code);
	void remove_key(uint8_t code);

	/* Data */
	std::vector<InputKey> * m_keys;
	float m_l_stick_x = 0.f, m_l_stick_y = 0.f;
	float m_r_stick_x = 0.f, m_r_stick_y = 0.f;
	uint16_t m_r_dead_zone = 0, m_l_dead_zone = 0;

	std::string m_device_path;
	FILE *m_file = nullptr;
    unsigned char m_packet[8];
};

#endif