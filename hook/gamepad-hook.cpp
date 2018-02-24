#include "gamepad-hook.hpp"

#ifdef LINUX

/**
 * This file is part of input-overlay
 * which is licenced under the MIT licence.
 * See LICENCE or https://mit-license.org
 * github.com/univrsal/input-overlay
 */

GamepadState pad_states[PAD_COUNT];
bool gamepad_hook_state = false;

static pthread_t game_pad_hook_thread;

void start_pad_hook(void)
{
    init_pad_devices();

    pthread_create(&game_pad_hook_thread, NULL, hook_method, NULL);
    gamepad_hook_state = true;
}

void init_pad_devices(void)
{
    pad_states[0].set_path("/dev/input/js0");
    pad_states[1].set_path("/dev/input/js1");
    pad_states[2].set_path("/dev/input/js2");
    pad_states[3].set_path("/dev/input/js3");
}

void end_pad_hook(void)
{
    pthread_cancel(game_pad_hook_thread);

    pad_states[0].unload();
    pad_states[1].unload();
    pad_states[2].unload();
    pad_states[3].unload();
}

#define ID_TYPE 		6
#define ID_BUTTON		1
#define ID_STATE_1		4
#define ID_STATE_2		5
#define ID_KEY_CODE		7
#define ID_PRESSED		1

#define ID_L_ANALOG_X	0
#define ID_L_ANALOG_Y	1
#define ID_L_TRIGGER	2
#define ID_R_ANALOG_X	3
#define ID_R_ANALOG_Y	4
#define ID_R_TRIGGER	5

#define STICK_MAX_VAL   127

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void * hook_method(void *)
{
    while(true)
    {
        for (int i = 0; i < PAD_COUNT; i++)
        {
            if (!pad_states[i].valid())
                continue;

            unsigned char m_packet[8];
            fread(m_packet, sizeof(char) * 8, 1, pad_states[i].dev());
            if (m_packet[ID_TYPE] == ID_BUTTON)
            {
                pad_states[i].set_key(m_packet[ID_STATE_1] == ID_PRESSED, m_packet[ID_KEY_CODE]);
            }
            else
            {
                switch(m_packet[ID_KEY_CODE])
                {
                    case ID_L_TRIGGER:
                        pad_states[i].set_key(m_packet[ID_STATE_1] > 127, PAD_LT);
                        break;
                    case ID_R_TRIGGER:
                        pad_states[i].set_key(m_packet[ID_STATE_1] > 127, PAD_RT);
                        break;
                    case ID_R_ANALOG_X:
                        if (m_packet[ID_STATE_1] == 0 || m_packet[ID_STATE_2] == 0)
                        {
                            pad_states[i].r_x = 0.f;
                            break;
                        }

                        if (m_packet[ID_STATE_2] < 128)
                            pad_states[i].r_x = UTIL_CLAMP(-1.f, ((float) m_packet[ID_STATE_2]) / STICK_MAX_VAL, 1.f);
                        else
                            pad_states[i].r_x = UTIL_CLAMP(-1.f, ((float) m_packet[ID_STATE_2] - 255) / STICK_MAX_VAL, 1.f);
                        break;
                    case ID_R_ANALOG_Y:
                        if (m_packet[ID_STATE_1] == 0 || m_packet[ID_STATE_2] == 0)
                        {
                            pad_states[i].r_y = 0.f;
                            break;
                        }

                        if (m_packet[ID_STATE_2] < 128)
                            pad_states[i].r_y = -1.f * UTIL_CLAMP(-1.f, ((float) m_packet[ID_STATE_2]) / STICK_MAX_VAL, 1.f);
                        else
                            pad_states[i].r_y = -1.f * UTIL_CLAMP(-1.f, ((float) m_packet[ID_STATE_2] - 255) / STICK_MAX_VAL, 1.f);
                        break;
                    case ID_L_ANALOG_X:
                        if (m_packet[ID_STATE_1] == 0 || m_packet[ID_STATE_2] == 0)
                        {
                            pad_states[i].l_x = 0.f;
                            break;
                        }

                        if (m_packet[ID_STATE_2] < 128)
                            pad_states[i].l_x = UTIL_CLAMP(-1.f, ((float) m_packet[ID_STATE_2]) / STICK_MAX_VAL, 1.f);
                        else
                            pad_states[i].l_x = UTIL_CLAMP(-1.f, ((float) m_packet[ID_STATE_2] - 255) / STICK_MAX_VAL, 1.f);
                        break;
                    case ID_L_ANALOG_Y:
                        if (m_packet[ID_STATE_1] == 0 || m_packet[ID_STATE_2] == 0)
                        {
                            pad_states[i].l_y = 0.f;
                            break;
                        }

                        if (m_packet[ID_STATE_2] < 128)
                            pad_states[i].l_y = -1.f * UTIL_CLAMP(-1.f, ((float) m_packet[ID_STATE_2]) / STICK_MAX_VAL, 1.f);
                        else
                            pad_states[i].l_y = -1.f * UTIL_CLAMP(-1.f, ((float) m_packet[ID_STATE_2] - 255) / STICK_MAX_VAL, 1.f);
                        break;
                }
            }

        }
    }
    return NULL;
}
#pragma clang diagnostic pop
#endif