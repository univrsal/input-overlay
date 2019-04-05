/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "xinput_fix.hpp"
#include "util/util.hpp"
#include "gamepad_binding.hpp"
#include <stdio.h>
#include <mutex>

namespace gamepad
{
    /* Linux implementation */
#ifdef LINUX
    extern gamepad_binding bindings;
#include <stdlib.h>
#include <string>
#include <malloc.h>
#include <unistd.h>

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
            m_device_file = nullptr;
        }

        void load()
        {
            m_device_file = fopen(m_path.c_str(), "wb+");
            if (m_device_file) {
                void* tmp = malloc(8 * 12 * sizeof(char));
                fread(tmp, sizeof(char) * 8 * 12, 1, m_device_file);
                free(tmp);
            }

#if _DEBUG
            blog(LOG_INFO, "Gamepad %i present: %s", m_pad_id, m_device_file ? "true" : "false");
#endif
        }

        bool valid()
        { return m_device_file != nullptr && m_pad_id >= 0; }

        void init(uint8_t pad_id)
        {
            unload();
            m_pad_id = pad_id;
            m_path.clear();
            m_path = "/dev/input/js";
            m_path.append(std::to_string(pad_id));
            load();
        }

        FILE* dev()
        { return m_device_file; }

        uint8_t get_id() const
        { return static_cast<uint8_t>(m_pad_id); }

    private:
        FILE* m_device_file = nullptr;
        std::string m_path;
        int8_t m_pad_id = -1;
    };

#endif /* LINUX */

    /* Windows implementation */
#ifdef _WIN32
    struct GamepadState
    {
        ~GamepadState()
        {
            unload();
        }

        void unload()
        {
            ZeroMemory(&m_xinput, sizeof(xinput_fix::gamepad));
        }

        void load()
        {
            unload();
            update();
        }

        void update()
        {
            if (xinput_fix::update(m_pad_id, &m_xinput) == ERROR_SUCCESS)
            {
                m_valid = true;
            }
            else
            {
                m_valid = false;
            }
        }

        bool valid()
        {
            update();
            return m_valid;
        }

        void init(const uint8_t pad_id)
        {
            m_pad_id = pad_id;
            load();
        }

        uint8_t get_id() const
        {
            return m_pad_id;
        }

        xinput_fix::gamepad* get_xinput()
        {
            return &m_xinput;
        }

    private:
        xinput_fix::gamepad m_xinput;
        bool m_valid = false;
        int8_t m_pad_id = -1;
    };

#endif /* HAVE_XINPUT */

    void start_pad_hook();

#ifdef _WIN32
    DWORD WINAPI hook_method(LPVOID arg);
#else
    void* hook_method(void*);
#endif

    void end_pad_hook();

    bool init_pad_devices();

    /* Mutex for thread safety */
    extern std::mutex mutex;
    /* Four structs containing info to query gamepads */
    extern GamepadState pad_states[PAD_COUNT];
    /* Init state of hook */
    extern bool gamepad_hook_state;
    /* False will end thread */
    extern bool gamepad_hook_run_flag; }
