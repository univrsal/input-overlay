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

namespace gamepad {
    bool gamepad_hook_state = false;
    GamepadState pad_states[PAD_COUNT];

#ifdef WINDOWS
    static HANDLE hook_thread;
#else
    static pthread_t game_pad_hook_thread;
#endif
    
    void start_pad_hook() {
        init_pad_devices();

#ifdef WINDOWS
        hook_thread = CreateThread(nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(hook_method),
            nullptr, 0, nullptr);
        gamepad_hook_state = hook_thread;
#else
        gamepad_hook_state = pthread_create(&game_pad_hook_thread, nullptr, hook_method, nullptr) == 0;
#endif
    }
    
    void init_pad_devices() {
        uint8_t id = 0;
        for (auto &state : pad_states)
            state.init(id++);
    }

#ifdef WINDOWS
    uint16_t xinput_to_vc(const uint16_t code)
    {
        switch(code)
        {
        case XINPUT_GAMEPAD_A: return PAD_TO_VC(PAD_A);
        case XINPUT_GAMEPAD_B: return PAD_TO_VC(PAD_B);
        case XINPUT_GAMEPAD_X: return PAD_TO_VC(PAD_X);
        case XINPUT_GAMEPAD_Y: return PAD_TO_VC(PAD_Y);
        case XINPUT_GAMEPAD_DPAD_DOWN: return PAD_TO_VC(PAD_DPAD_DOWN);
        case XINPUT_GAMEPAD_DPAD_UP: return PAD_TO_VC(PAD_DPAD_UP);
        case XINPUT_GAMEPAD_DPAD_LEFT: return PAD_TO_VC(PAD_DPAD_LEFT);
        case XINPUT_GAMEPAD_DPAD_RIGHT: return PAD_TO_VC(PAD_DPAD_RIGHT);
        case XINPUT_GAMEPAD_LEFT_SHOULDER: return PAD_TO_VC(PAD_LB);
        case XINPUT_GAMEPAD_RIGHT_SHOULDER: return PAD_TO_VC(PAD_RB);
        case XINPUT_GAMEPAD_START: return PAD_TO_VC(PAD_START);
        case XINPUT_GAMEPAD_BACK: return PAD_TO_VC(PAD_BACK);
        default: return 0xFFFF;
        }
    }
#endif
    
    void end_pad_hook() {
#ifdef WINDOWS
        CloseHandle(hook_thread);
#else
        pthread_cancel(game_pad_hook_thread);
#endif
        for (auto &state : pad_states)
            state.unload();
    }
    
    /* Background process for quering game pads */
#ifdef WINDOWS
    DWORD WINAPI hook_method(const LPVOID arg)
#else
    
    void *hook_method(void *)
#endif
    {
        while (true) {
            if (!hook::input_data)
                break;
            for (auto &pad : pad_states) {
                if (!pad.valid())
                    continue;

#ifdef WINDOWS
                for (const auto& button : pad_keys)
                {
                    const auto state = X_PRESSED(button)
                        ? STATE_PRESSED
                        : STATE_RELEASED;
                    hook::input_data->add_gamepad_data(
                        pad.get_id(), xinput_to_vc(button),
                        new element_data_button(state));
                }

                /* Analog sticks */
                hook::input_data->add_gamepad_data(pad.get_id(), VC_STICK_DATA,
                    new element_data_analog_stick(
                        X_PRESSED(XINPUT_GAMEPAD_LEFT_THUMB)
                        ? STATE_PRESSED
                        : STATE_RELEASED,
                         X_PRESSED(XINPUT_GAMEPAD_RIGHT_THUMB)
                        ? STATE_PRESSED
                        : STATE_RELEASED,
                        pad.get_xinput()->Gamepad.sThumbLX /
                        STICK_MAX_VAL,
                        -pad.get_xinput()->Gamepad.sThumbLY /
                        STICK_MAX_VAL,
                        pad.get_xinput()->Gamepad.sThumbRX /
                        STICK_MAX_VAL,
                        -pad.get_xinput()->Gamepad.sThumbRY /
                        STICK_MAX_VAL
                    ));

                /* Trigger buttons */
                hook::input_data->add_gamepad_data(pad.get_id(), VC_TRIGGER_DATA,
                    new element_data_trigger(
                        pad.get_xinput()->Gamepad.bLeftTrigger /
                        255.f,
                        pad.get_xinput()->Gamepad.bRightTrigger /
                        255.f
                    ));
#else
                unsigned char m_packet[8];
                fread(m_packet, sizeof(char) * 8, 1, pad.dev());
                
                if (m_packet[ID_TYPE] == ID_BUTTON) {
                    hook::input_data->add_gamepad_data(pad.get_id(),
                        PAD_TO_VC(m_packet[ID_KEY_CODE]),
                        new element_data_button(
                        m_packet[ID_STATE_1] == ID_PRESSED ?
                        STATE_PRESSED : STATE_RELEASED
                        ));
                } else {
                    float axis = 0.f;
                    switch (m_packet[ID_KEY_CODE]) {
                        case ID_L_TRIGGER:
                            hook::input_data->add_gamepad_data(pad.get_id(), VC_TRIGGER_DATA,
                                new element_data_trigger(SIDE_LEFT,
                                    m_packet[ID_STATE_1] / 255.f));
                            break;
                        case ID_R_TRIGGER:
                            hook::input_data->add_gamepad_data(pad.get_id(), VC_TRIGGER_DATA,
                                new element_data_trigger(SIDE_RIGHT,
                                m_packet[ID_STATE_1] / 255.f));
                            break;
                        case ID_R_ANALOG_X:
                            if (m_packet[ID_STATE_2] < 128)
                                axis = UTIL_CLAMP(-1.f, (m_packet[ID_STATE_2]) / STICK_MAX_VAL, 1.f);
                            else
                                axis = UTIL_CLAMP(-1.f,
                                    (m_packet[ID_STATE_2] - 255) / STICK_MAX_VAL, 1.f);
                            
                            hook::input_data->add_gamepad_data(pad.get_id(), VC_STICK_DATA, new element_data_analog_stick(axis,
                                STICK_STATE_RIGHT_X));
                            break;
                        case ID_R_ANALOG_Y:
                            if (m_packet[ID_STATE_2] < 128)
                                axis = -1.f * UTIL_CLAMP(-1.f, (m_packet[ID_STATE_2]) / STICK_MAX_VAL, 1.f);
                            else
                                axis = -1.f * UTIL_CLAMP(-1.f, (m_packet[ID_STATE_2] - 255) /
                                STICK_MAX_VAL, 1.f);
                            hook::input_data->add_gamepad_data(pad.get_id(), VC_STICK_DATA, new element_data_analog_stick(axis,
                                STICK_STATE_RIGHT_Y));
                            break;
                        case ID_L_ANALOG_X:
                            if (m_packet[ID_STATE_2] < 128)
                                axis = UTIL_CLAMP(-1.f, (m_packet[ID_STATE_2]) / STICK_MAX_VAL, 1.f);
                            else
                                axis = UTIL_CLAMP(-1.f, (m_packet[ID_STATE_2] - 255) / STICK_MAX_VAL, 1.f);
        
                            hook::input_data->add_gamepad_data(pad.get_id(), VC_STICK_DATA, new element_data_analog_stick(axis,
                                STICK_STATE_LEFT_X));
                            break;
                        case ID_L_ANALOG_Y:
                            if (m_packet[ID_STATE_2] < 128)
                                axis =
                                    -1.f * UTIL_CLAMP(-1.f, ((float) m_packet[ID_STATE_2]) / STICK_MAX_VAL, 1.f);
                            else
                                axis = -1.f * UTIL_CLAMP(-1.f, ((float) m_packet[ID_STATE_2] - 255) / STICK_MAX_VAL, 1.f);
        
                            hook::input_data->add_gamepad_data(pad.get_id(), VC_STICK_DATA, new element_data_analog_stick(axis,
                                STICK_STATE_RIGHT_Y));
                            break;
                    }
                }

#endif /* LINUX */
            }
#ifdef  WINDOWS /* Delay on linux results in buffered input */
            os_sleep_ms(25);
#endif
        }
#ifdef WINDOWS
        return UIOHOOK_SUCCESS;
#else
        return nullptr;
#endif
    }
}
