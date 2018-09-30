/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "../util/util.hpp"
#include "hook_helper.hpp"

namespace gamepad
{
    extern bool gamepad_hook_state;

    /* Linux implementation */
#ifdef LINUX
#define ID_TYPE         6
#define ID_BUTTON       1
#define ID_STATE_1      4
#define ID_STATE_2      5
#define ID_KEY_CODE     7
#define ID_PRESSED      1

#define ID_L_ANALOG_X   0
#define ID_L_ANALOG_Y   1
#define ID_L_TRIGGER    2
#define ID_R_ANALOG_X   3
#define ID_R_ANALOG_Y   4
#define ID_R_TRIGGER    5

#define STICK_MAX_VAL   127.f

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <malloc.h>

struct GamepadState
{
	~GamepadState()
	{
		unload();
	}

	void unload()
	{
		if (m_device_file)
			fclose(m_device_file);
	}

	void load()
	{
		m_device_file = fopen(m_path.c_str(), "wb+");
		if (m_device_file)
		{
			void * tmp = malloc(8 * 12 * sizeof(char));
			fread(tmp, sizeof(char) * 8 * 12, 1, m_device_file);
			free(tmp);
		}
	}

	bool valid() { return m_device_file != NULL; }

	void init(uint8_t pad_id)
	{
		m_path.clear();
		m_path.append("/dev/input/js");
		m_path.append(std::to_string(pad_id));
		load();
	}

	FILE * dev() { return m_device_file; }

private:
	FILE * m_device_file = nullptr;
	std::string m_path;
};
#endif

    /* Windows implementation */
#ifdef HAVE_XINPUT

#include <Xinput.h>
#include <windows.h>

#define X_PRESSED(b)        ((pad.get_xinput()->Gamepad.wButtons & b) != 0)
#define STICK_MAX_VAL       32767.f

    static uint16_t pad_keys[] =
    {
        XINPUT_GAMEPAD_A,
        XINPUT_GAMEPAD_B,
        XINPUT_GAMEPAD_X,
        XINPUT_GAMEPAD_Y,
        XINPUT_GAMEPAD_DPAD_DOWN,
        XINPUT_GAMEPAD_DPAD_UP,
        XINPUT_GAMEPAD_DPAD_LEFT,
        XINPUT_GAMEPAD_DPAD_RIGHT,
        XINPUT_GAMEPAD_LEFT_SHOULDER,
        XINPUT_GAMEPAD_RIGHT_SHOULDER,
        XINPUT_GAMEPAD_START,
        XINPUT_GAMEPAD_BACK
    };

    struct GamepadState
    {
        ~GamepadState()
        {
            unload();
        }

        void unload()
        {
            ZeroMemory(&m_xinput, sizeof(XINPUT_STATE));
        }

        void load()
        {
            unload();
            update();
        }

        void update()
        {
            if (XInputGetState(m_pad_id, &m_xinput) == ERROR_SUCCESS)
            {
                m_valid = true;
            }
            else
            {
                m_valid = false;
            }
        }

        bool valid() { update(); return m_valid; }

        void init(const uint8_t pad_id)
        {
            m_pad_id = pad_id;
            load();
        }

        uint8_t get_id() const
        {
            return m_pad_id;
        }

        XINPUT_STATE* get_xinput()
        {
            return &m_xinput;
        }

    private:
        XINPUT_STATE m_xinput;
        bool m_valid = false;
        int8_t m_pad_id = -1;
    };

#endif // HAVE_XINPUT

    /* Four structs containing info to query gamepads */
    extern GamepadState pad_states[PAD_COUNT];

    void start_pad_hook();

#ifdef WINDOWS
    DWORD WINAPI hook_method(const LPVOID arg);
#else
    void* hook_method(void*);
#endif

#ifdef WINDOWS
    static uint16_t xinput_to_vc(uint16_t code);
#endif
    void end_pad_hook();

    void init_pad_devices();
}
