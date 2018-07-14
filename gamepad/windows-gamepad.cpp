#include "windows-gamepad.hpp"

#if HAVE_XINPUT
#if 0
/**
 * This file is part of input-overlay
 * which is licenced under the MIT licence.
 * See LICENCE or https://mit-license.org
 * github.com/univrsal/input-overlay
 */

void WindowsGamepad::load()
{
	unload();
	if (XInputGetState(m_pad_id, &m_xinput) == ERROR_SUCCESS)
	{
		m_valid = true;
	}
	else
	{
		m_valid = false;
	}
}

void WindowsGamepad::unload()
{
	ZeroMemory(&m_xinput, sizeof(XINPUT_STATE));
}

void WindowsGamepad::update(uint8_t id, uint16_t r_dz, uint16_t l_dz)
{
	m_pad_id = id;
	m_r_dead_zone = r_dz;
	m_l_dead_zone = l_dz;
	load();
}

void WindowsGamepad::check_keys()
{
	load();
	if (m_valid)
	{
		(*m_keys)[PAD_L_ANALOG].m_pressed = X_PRESSED(XINPUT_GAMEPAD_LEFT_THUMB);
		(*m_keys)[PAD_R_ANALOG].m_pressed = X_PRESSED(XINPUT_GAMEPAD_RIGHT_THUMB);

		(*m_keys)[PAD_BACK].m_pressed = X_PRESSED(XINPUT_GAMEPAD_BACK);
		(*m_keys)[PAD_START].m_pressed = X_PRESSED(XINPUT_GAMEPAD_START);

		(*m_keys)[PAD_X].m_pressed = X_PRESSED(XINPUT_GAMEPAD_X);
		(*m_keys)[PAD_Y].m_pressed = X_PRESSED(XINPUT_GAMEPAD_Y);
		(*m_keys)[PAD_A].m_pressed = X_PRESSED(XINPUT_GAMEPAD_A);
		(*m_keys)[PAD_B].m_pressed = X_PRESSED(XINPUT_GAMEPAD_B);

		(*m_keys)[PAD_LB].m_pressed = X_PRESSED(XINPUT_GAMEPAD_LEFT_SHOULDER);
		(*m_keys)[PAD_LT].m_pressed = m_xinput.Gamepad.bLeftTrigger > 20;

		(*m_keys)[PAD_RB].m_pressed = X_PRESSED(XINPUT_GAMEPAD_RIGHT_SHOULDER);
		(*m_keys)[PAD_RT].m_pressed = m_xinput.Gamepad.bRightTrigger > 20;

		(*m_keys)[PAD_DPAD_UP].m_pressed = X_PRESSED(XINPUT_GAMEPAD_DPAD_UP);
		(*m_keys)[PAD_DPAD_DOWN].m_pressed = X_PRESSED(XINPUT_GAMEPAD_DPAD_DOWN);
		(*m_keys)[PAD_DPAD_LEFT].m_pressed = X_PRESSED(XINPUT_GAMEPAD_DPAD_LEFT);
		(*m_keys)[PAD_DPAD_RIGHT].m_pressed = X_PRESSED(XINPUT_GAMEPAD_DPAD_RIGHT);

		if (!DEAD_ZONE(m_xinput.Gamepad.sThumbLX, m_l_dead_zone))
			m_l_stick_x = fmaxf(-1, (float)m_xinput.Gamepad.sThumbLX / PAD_STICK_MAX_VAL);
		else
			m_l_stick_x = 0.f;

		if (!DEAD_ZONE(m_xinput.Gamepad.sThumbLY, m_l_dead_zone))
			m_l_stick_y = fmaxf(-1, (float)m_xinput.Gamepad.sThumbLY / PAD_STICK_MAX_VAL);
		else
			m_l_stick_y = 0.f;

		if (!DEAD_ZONE(m_xinput.Gamepad.sThumbRX, m_r_dead_zone))
			m_r_stick_x = fmaxf(-1, (float)m_xinput.Gamepad.sThumbRX / PAD_STICK_MAX_VAL);
		else
			m_r_stick_x = 0.f;

		if (!DEAD_ZONE(m_xinput.Gamepad.sThumbRY, m_r_dead_zone))
			m_r_stick_y = fmaxf(-1, (float)m_xinput.Gamepad.sThumbRY / PAD_STICK_MAX_VAL);
		else
			m_r_stick_y = 0.f;
	}
}
#endif

#endif // HAVE_XINPUT
