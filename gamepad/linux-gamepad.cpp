#include "linux-gamepad.hpp"

void LinuxGamepad::load()
{
	unload();

	m_file = fopen(m_device_path.c_str(), "wb+");
	
	blog(LOG_WARNING, "FILE IS der 0x%A", m_file);

	if (m_file)
	{
		// Skip unnecessary info
		void *tmp = malloc(8 * 12 * sizeof(char));
		fread(tmp, sizeof(char) * 8 * 12, 1, m_file);
		free(tmp);
	}
}

void LinuxGamepad::unload()
{
	if (m_file)
	{
		fclose(m_file);
		m_file = nullptr;
	}
}

void LinuxGamepad::update(std::string path, uint16_t r_dz, uint16_t l_dz)
{
	m_device_path = path;
	m_r_dead_zone = r_dz;
	m_l_dead_zone = l_dz;
	
	if (!path.empty())
		load();
}

void LinuxGamepad::add_key(uint8_t code)
{
	(*m_keys)[code].m_pressed = true;
}

void LinuxGamepad::remove_key(uint8_t code)
{
	(*m_keys)[code].m_pressed = false;
}

#define ID_TYPE 		6
#define ID_BUTTON		1
#define ID_STATE_1		4
#define ID_STATE_2		5
#define ID_KEY_CODE		7
#define ID_PRESSED		1

#define ID_R_ANALOG_X	0
#define ID_R_ANALOG_Y	1
#define ID_L_TRIGGER	2
#define ID_L_ANALOG_X	3
#define ID_L_ANALOG_Y	4
#define ID_R_TRIGGER	5

#define STICK_MAX_VAL   255

void LinuxGamepad::check_keys()
{
	if (!m_file)
	{
		return;
	}
	blog(LOG_WARNING, "VALID...");
	
	fread(&m_packet, sizeof(char) * 8, 1, m_file);

	blog(LOG_WARNING, "STATE: %i, STATE 2: %i, TYPE: %i, KEY: %i", m_packet[4],
		m_packet[5], m_packet[6], m_packet[7]);

	if (m_packet[ID_TYPE] == ID_BUTTON)
	{
		if (m_packet[ID_STATE_1] == ID_PRESSED)
		{
			add_key(m_packet[ID_KEY_CODE]);
		}
		else
		{
			remove_key(m_packet[ID_KEY_CODE]);
		}
	}
	else
	{
		switch(m_packet[ID_KEY_CODE])
		{
			case ID_L_TRIGGER:
				if (m_packet[ID_STATE_1] > 127)
					add_key(PAD_LT);
				else
					remove_key(PAD_LT);
				break;
			case ID_R_TRIGGER:
				if (m_packet[ID_STATE_1] > 127)
					add_key(PAD_RT);
				else
					remove_key(PAD_RT);
				break;
			case ID_R_ANALOG_X:
				m_r_stick_x = fmaxf(-1, (float)
					m_packet[ID_STATE_1] / STICK_MAX_VAL);
				break;
			case ID_R_ANALOG_Y:
				m_r_stick_y = fmaxf(-1, (float)
					m_packet[ID_STATE_1] / STICK_MAX_VAL);
				break;
			case ID_L_ANALOG_X:
				m_l_stick_x = fmaxf(-1, (float)
					m_packet[ID_STATE_1] / STICK_MAX_VAL);
				break;
			case ID_L_ANALOG_Y:
				m_l_stick_y = fmaxf(-1, (float)
					m_packet[ID_STATE_1] / STICK_MAX_VAL);
				break;
		}
	}
}