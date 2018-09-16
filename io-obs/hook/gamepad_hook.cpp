/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include <util/platform.h>
#include "gamepad_hook.hpp"
#include "hook_helper.hpp"
#include "../util/element/element_data_holder.hpp"
#include "../util/element/element_button.hpp"
#include "../util/element/element_analog_stick.hpp"
#include "../util/element/element_trigger.hpp"

namespace gamepad
{
    bool gamepad_hook_state = false;
    GamepadState pad_states[PAD_COUNT];

#ifdef WINDOWS
static HANDLE hook_thread;
#else
static pthread_t game_pad_hook_thread;
#endif

    void start_pad_hook()
    {
        init_pad_devices();

#ifdef WINDOWS
        hook_thread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE) hook_method,
            nullptr, 0, nullptr);
        gamepad_hook_state = hook_thread;
#else
	    gamepad_hook_state = pthread_create(&game_pad_hook_thread, NULL, hook_method, NULL) == 0;
#endif
    }

    void init_pad_devices()
    {
        uint8_t id = 0;
        for (auto& state : pad_states)
            state.init(id++);
    }

    void end_pad_hook()
    {
#ifdef WINDOWS
        CloseHandle(hook_thread);
#else
	    pthread_cancel(game_pad_hook_thread);
#endif
        for (auto& state : pad_states)
            state.unload();
    }

    /* Background process for quering game pads */
#ifdef WINDOWS
     DWORD WINAPI hook_method(const LPVOID arg)
#else
    void* hook_method(void*)
#endif
    {
        while (true)
        {
            if (!hook::input_data)
                break;
            for (auto& pad : pad_states)
            {
                if (!pad.valid())
                    continue;

#ifdef WINDOWS
                for (const auto& button : pad_keys)
                {
                    const auto state = X_PRESSED(button)
                        ? STATE_PRESSED
                        : STATE_RELEASED;
                    hook::input_data->add_gamepad_data(
                        pad.get_id(), PAD_TO_VC(button),
                        new element_data_button(state));
                }

                /* Analog sticks */
                hook::input_data->add_gamepad_data(pad.get_id(), VC_STICK_DATA,
                    new element_data_analog_stick(
                        X_PRESSED(XINPUT_GAMEPAD_LEFT_THUMB)
                        ? STATE_PRESSED
                        : STATE_RELEASED,
                        static_cast<float>(pad.get_xinput()->Gamepad.sThumbLX) /
                        STICK_MAX_VAL,
                        static_cast<float>(pad.get_xinput()->Gamepad.sThumbLY) /
                        STICK_MAX_VAL,
                        static_cast<float>(pad.get_xinput()->Gamepad.sThumbRX) /
                        STICK_MAX_VAL,
                        static_cast<float>(pad.get_xinput()->Gamepad.sThumbRY) /
                        STICK_MAX_VAL
                    ));

                /* Trigger buttons */
                hook::input_data->add_gamepad_data(pad.get_id(), VC_TRIGGER_DATA,
                    new element_data_trigger(
                        static_cast<float>(pad.get_xinput()->Gamepad.bLeftTrigger) /
                        255,
                        static_cast<float>(pad.get_xinput()->Gamepad.bRightTrigger) /
                        255
                    ));
#else
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
		
	
#endif /* LINUX */
            }
            os_sleep_ms(100);
        }
#ifdef WINDOWS
        return UIOHOOK_SUCCESS;
#else
        return nullptr;
#endif
    }
}
