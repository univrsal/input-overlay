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
#include "../util/element/element_dpad.hpp"

namespace gamepad {
    bool gamepad_hook_state = false;
    bool gamepad_hook_run_flag = true;
    GamepadState pad_states[PAD_COUNT];

#ifdef WINDOWS
    static HANDLE hook_thread;
#else
    static pthread_t game_pad_hook_thread;
#endif
    
    void start_pad_hook()
    {
        if (gamepad_hook_state)
            return;
#ifdef LINUX
        /* Prevent the hook loop from running if there's no gamepads
         * Prevents infinite loop
         * Hook will be restarted, once users click "reload" button
         * in source properties
         */
        gamepad_hook_state = gamepad_hook_run_flag = init_pad_devices();    
#endif

#ifdef WINDOWS
        hook_thread = CreateThread(nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(hook_method),
            nullptr, 0, nullptr);
        gamepad_hook_state = hook_thread;
#else
        gamepad_hook_state = pthread_create(&game_pad_hook_thread, nullptr, hook_method, nullptr) == 0;
#endif
    }
    
    bool init_pad_devices()
    {
        
        uint8_t id = 0;
        auto flag = false;
        for (auto &state : pad_states)
        {
            state.init(id++);
            if (state.valid())
                flag = true;
        }
        return flag;
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
        gamepad_hook_run_flag = false;

#ifdef WINDOWS
        CloseHandle(hook_thread);
#endif
    }
    
    /* Background process for quering game pads */
#ifdef WINDOWS
    DWORD WINAPI hook_method(const LPVOID arg)
#else
    
    void* hook_method(void *)
#endif
    {
        while (gamepad_hook_run_flag) {
            if (!hook::input_data)
                break;
            for (auto &pad : pad_states) {
                if (!pad.valid())
                    continue;

#ifdef WINDOWS
                dpad_direction dir[] = { DPAD_CENTER, DPAD_CENTER };

                for (const auto& button : pad_keys)
                {
                    const auto state = X_PRESSED(button)
                        ? STATE_PRESSED
                        : STATE_RELEASED;
                    hook::input_data->add_gamepad_data(
                        pad.get_id(), xinput_to_vc(button),
                        new element_data_button(state));

                    if (state == STATE_PRESSED)
                    {
                        switch(button)
                        {
                        case XINPUT_GAMEPAD_DPAD_UP:
                            if (!dir[0])
                                dir[0] = DPAD_UP;
                            else
                                dir[1] = DPAD_UP;
                            break;                    
                        case XINPUT_GAMEPAD_DPAD_DOWN:
                            if (!dir[0])
                                dir[0] = DPAD_DOWN;
                            else
                                dir[1] = DPAD_DOWN;
                            break;                    
                        case XINPUT_GAMEPAD_DPAD_LEFT:
                            if (!dir[0])
                                dir[0] = DPAD_LEFT;
                            else
                                dir[1] = DPAD_LEFT;
                            break;
                        case XINPUT_GAMEPAD_DPAD_RIGHT:
                            if (!dir[0])
                                dir[0] = DPAD_RIGHT;
                            else
                                dir[1] = DPAD_RIGHT;
                            break;
                        default: ;
                        }   
                    }
                }

                /* Dpad direction */
                hook::input_data->add_gamepad_data(pad.get_id(), VC_DPAD_DATA,
                    new element_data_dpad(dir[0], dir[1]));

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
                    switch(m_packet[ID_KEY_CODE])
                    {
                        case PAD_L_ANALOG:
                            hook::input_data->add_gamepad_data(pad.get_id(), VC_STICK_DATA,
                                new element_data_analog_stick(m_packet[ID_STATE_1] == ID_PRESSED ?
                                STATE_PRESSED : STATE_RELEASED, SIDE_LEFT));
                            break;
                        case PAD_R_ANALOG:
                            hook::input_data->add_gamepad_data(pad.get_id(), VC_STICK_DATA,
                                new element_data_analog_stick(m_packet[ID_STATE_1] == ID_PRESSED ?
                                STATE_PRESSED : STATE_RELEASED, SIDE_RIGHT));
                            break;
                        default:
                            switch(m_packet[ID_KEY_CODE])
                            {
                                case PAD_DPAD_DOWN:
                                    hook::input_data->add_gamepad_data(pad.get_id(),
                                        VC_DPAD_DATA, new element_data_dpad(
                                            DPAD_DOWN, m_packet[ID_STATE_1] == ID_PRESSED ?
                                                       STATE_PRESSED : STATE_RELEASED
                                            ));
                                break;
                                case PAD_DPAD_UP:
                                    hook::input_data->add_gamepad_data(pad.get_id(),
                                                                       VC_DPAD_DATA, new element_data_dpad(
                                            DPAD_UP, m_packet[ID_STATE_1] == ID_PRESSED ?
                                                       STATE_PRESSED : STATE_RELEASED
                                        ));
                                    break;
                                case PAD_DPAD_LEFT:
                                    hook::input_data->add_gamepad_data(pad.get_id(),
                                        VC_DPAD_DATA, new element_data_dpad(
                                            DPAD_LEFT, m_packet[ID_STATE_1] == ID_PRESSED ?
                                                       STATE_PRESSED : STATE_RELEASED
                                        ));
                                    break;
                                case PAD_DPAD_RIGHT:
                                    hook::input_data->add_gamepad_data(pad.get_id(),
                                        VC_DPAD_DATA, new element_data_dpad(
                                            DPAD_RIGHT, m_packet[ID_STATE_1] == ID_PRESSED ?
                                                       STATE_PRESSED : STATE_RELEASED
                                        ));
                                    break;
                                default: ;
                            }
                            hook::input_data->add_gamepad_data(pad.get_id(),
                                PAD_TO_VC(m_packet[ID_KEY_CODE]),
                                new element_data_button(m_packet[ID_STATE_1] == ID_PRESSED ?
                                STATE_PRESSED : STATE_RELEASED));
                    }
                } else {
                    float axis;
                    switch (m_packet[ID_KEY_CODE]) {
                        case ID_L_TRIGGER:
                            hook::input_data->add_gamepad_data(pad.get_id(), VC_TRIGGER_DATA,
                                new element_data_trigger(T_DATA_LEFT,
                                    m_packet[ID_STATE_1] / 255.f));
                            break;
                        case ID_R_TRIGGER:
                            hook::input_data->add_gamepad_data(pad.get_id(), VC_TRIGGER_DATA,
                                new element_data_trigger(T_DATA_RIGHT,
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
                                axis = UTIL_CLAMP(-1.f, (m_packet[ID_STATE_2]) / STICK_MAX_VAL, 1.f);
                            else
                                axis = UTIL_CLAMP(-1.f, (m_packet[ID_STATE_2] - 255) /
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
                                axis = UTIL_CLAMP(-1.f, ((float) m_packet[ID_STATE_2]) / STICK_MAX_VAL, 1.f);
                            else
                                axis = UTIL_CLAMP(-1.f, ((float) m_packet[ID_STATE_2] - 255) / STICK_MAX_VAL, 1.f);
        
                            hook::input_data->add_gamepad_data(pad.get_id(), VC_STICK_DATA, new element_data_analog_stick(axis,
                                STICK_STATE_LEFT_Y));
                            break;
                        default: ;
                    }
                }

#endif /* LINUX */
            }
#ifdef  WINDOWS /* Delay on linux results in buffered input */
            os_sleep_ms(25);
#endif
        }
    
        for (auto &state : pad_states)
            state.unload();
#ifdef WINDOWS
        return UIOHOOK_SUCCESS;
#else
        pthread_exit(nullptr);
#endif
    }
}
