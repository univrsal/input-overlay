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
    extern uint8_t last_input; /* Used in config screen to bind buttons */

#include <stdlib.h>
#include <string>
#include <malloc.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/joystick.h>

#define ID_TYPE         6
#define ID_BUTTON       1
#define ID_STATE_1      4
#define ID_STATE_2      5
#define ID_KEY_CODE     7
#define ID_PRESSED      1

    /* Utility methods
       From: https://gist.github.com/jasonwhite/c5b2048c15993d285130
     */

    struct axis_state
    {
        short x, y;
    };

    struct GamepadState
    {
        ~GamepadState()
        {
            unload();
        }

        void unload()
        {
            close(m_pad_id);
        }

        void load()
        {
            m_pad_id = open(m_path.c_str(), O_RDONLY);
#if _DEBUG
            blog(LOG_INFO, "Gamepad %i present: %s", m_pad_id, valid() ? "true" : "false");
#endif
        }

        bool valid()
        { return m_pad_id >= 0; }

        void init(uint8_t pad_id)
        {
            unload();
            m_path.clear();
            m_path = "/dev/input/js";
            m_path.append(std::to_string(pad_id));
            load();
        }

        int get_id() const
        { return m_pad_id; }

        int read_event()
        {
            ssize_t bytes;
            bytes = read(m_pad_id, &m_event, sizeof(m_event));
            if (bytes == sizeof(m_event))
                return 0;

            /* Error, could not read full event. */
            return -1;
        }

        struct js_event* get_event() { return &m_event; }
    private:
        std::string m_path;
        int m_pad_id = -1;
        struct js_event m_event;
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
