/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "gamepad_hook.hpp"

bool gamepad_hook_state = false;
GamepadState pad_states[PAD_COUNT];

#ifdef LINUX
static pthread_t game_pad_hook_thread;
#endif

void start_pad_hook()
{
    init_pad_devices();

#ifdef LINUX
	pthread_create(&game_pad_hook_thread, NULL, hook_method, NULL);
	gamepad_hook_state = true;
#endif
}

void init_pad_devices()
{
#ifdef LINUX
	pad_states[0].init(0);
	pad_states[1].init(1);
	pad_states[2].init(2);
	pad_states[3].init(3);
#endif
}

void end_pad_hook()
{
#ifdef LINUX
	pthread_cancel(game_pad_hook_thread);


	pad_states[0].unload();
	pad_states[1].unload();
	pad_states[2].unload();
	pad_states[3].unload();
#endif
}

#ifdef HAVE_XINPUT
void update_gamepads()
{
    for (int i = 0; i < PAD_COUNT; i++)
    {
        //pad_states[i].put_in_vc();
    }
}
#endif

float get_stick_value_x(uint8_t pad_id, bool left)
{
    if (pad_id < 0 || pad_id >= PAD_COUNT)
        return 0.f;

    if (left)
        return pad_states[pad_id].l_x;
    return pad_states[pad_id].r_x;
}

float get_stick_value_y(uint8_t pad_id, bool left)
{
    if (pad_id < 0 || pad_id >= PAD_COUNT)
        return 0.f;

    if (left)
        return pad_states[pad_id].l_y;
    return pad_states[pad_id].r_y;
}

/* Linux background process for game pads */
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

#define STICK_MAX_VAL   127

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void * hook_method(void *)
{
	while (true)
	{
		for (int i = 0; i < PAD_COUNT; i++)
		{
			if (!pad_states[i].valid())
				continue;

			unsigned char m_packet[8];
			fread(m_packet, sizeof(char) * 8, 1, pad_states[i].dev());

			if (m_packet[ID_TYPE] == ID_BUTTON)
			{
				util_set_pad_state(m_packet[ID_STATE_1], i, m_packet[ID_STATE_1] == ID_PRESSED);
			}
			else
			{
				switch (m_packet[ID_KEY_CODE])
				{
				case ID_L_TRIGGER:
					util_set_pad_state(PAD_LT, i, m_packet[ID_STATE_1] > 127);
					break;
				case ID_R_TRIGGER:
					util_set_pad_state(PAD_RT, i, m_packet[ID_STATE_1] > 127);
					break;
				case ID_R_ANALOG_X:
					if (m_packet[ID_STATE_1] == 0 || m_packet[ID_STATE_2] == 0)
					{
						pad_states[i].r_x = 0.f;
						break;
					}

					if (m_packet[ID_STATE_2] < 128)
						pad_states[i].r_x = UTIL_CLAMP(-1.f, ((float)m_packet[ID_STATE_2]) / STICK_MAX_VAL, 1.f);
					else
						pad_states[i].r_x = UTIL_CLAMP(-1.f, ((float)m_packet[ID_STATE_2] - 255) / STICK_MAX_VAL, 1.f);
					break;
				case ID_R_ANALOG_Y:
					if (m_packet[ID_STATE_1] == 0 || m_packet[ID_STATE_2] == 0)
					{
						pad_states[i].r_y = 0.f;
						break;
					}

					if (m_packet[ID_STATE_2] < 128)
						pad_states[i].r_y = -1.f * UTIL_CLAMP(-1.f, ((float)m_packet[ID_STATE_2]) / STICK_MAX_VAL, 1.f);
					else
						pad_states[i].r_y = -1.f * UTIL_CLAMP(-1.f, ((float)m_packet[ID_STATE_2] - 255) / STICK_MAX_VAL, 1.f);
					break;
				case ID_L_ANALOG_X:
					if (m_packet[ID_STATE_1] == 0 || m_packet[ID_STATE_2] == 0)
					{
						pad_states[i].l_x = 0.f;
						break;
					}

					if (m_packet[ID_STATE_2] < 128)
						pad_states[i].l_x = UTIL_CLAMP(-1.f, ((float)m_packet[ID_STATE_2]) / STICK_MAX_VAL, 1.f);
					else
						pad_states[i].l_x = UTIL_CLAMP(-1.f, ((float)m_packet[ID_STATE_2] - 255) / STICK_MAX_VAL, 1.f);
					break;
				case ID_L_ANALOG_Y:
					if (m_packet[ID_STATE_1] == 0 || m_packet[ID_STATE_2] == 0)
					{
						pad_states[i].l_y = 0.f;
						break;
					}

					if (m_packet[ID_STATE_2] < 128)
						pad_states[i].l_y = -1.f * UTIL_CLAMP(-1.f, ((float)m_packet[ID_STATE_2]) / STICK_MAX_VAL, 1.f);
					else
						pad_states[i].l_y = -1.f * UTIL_CLAMP(-1.f, ((float)m_packet[ID_STATE_2] - 255) / STICK_MAX_VAL, 1.f);
					break;
				}
			}
		}
	}
	return NULL;
}
#pragma clang diagnostic pop
#endif // LINUX
