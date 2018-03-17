#ifndef GAMEPAD_HOOK_HPP
#define GAMEPAD_HOOK_HPP


#include "../util/util.hpp"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <malloc.h>

/**
 * This file is part of input-overlay
 * which is licenced under the MIT licence.
 * See LICENCE or https://mit-license.org
 * github.com/univrsal/input-overlay
 */

extern GamepadState pad_states[PAD_COUNT]; // Only monitor four gamepads, I mean who even has more than four friends

/* Linux implementation */

#ifdef LINUX

extern bool gamepad_hook_state;

const char* device_path = "/dev/input/js";

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

    float l_x, l_y, r_x, r_y;
    
    void init(uint8_t pad_id)
    { 
        m_path.clear();
        m_path.append(device_path);
        m_path.append(std::to_string(pad_id));
        load();
    }

    /* Linux specific */
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

#define DEAD_ZONE(x, dz) ((x < dz) && (x > -dz))
#define X_PRESSED(b) ((m_xinput.Gamepad.wButtons & b) != 0)

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
        if (XInputGetState(m_pad_id, &m_xinput) == ERROR_SUCCESS)
        {
            m_valid = true;
        }
        else
        {
            m_valid = false;
        }
    }

    bool valid() { return m_valid; }

    float l_x, l_y, r_x, r_y;

    void init(uint8_t pad_id)
    { 
        m_pad_id = pad_id;
        load();
    }

    /* Windows specific stuff */

    // Writes all pressed buttons into the global
    // array
    void put_in_vc(void)
    {
        util_set_pad_state(PAD_L_ANALOG, m_pad_id, X_PRESSED(XINPUT_GAMEPAD_LEFT_THUMB));
        util_set_pad_state(PAD_R_ANALOG, m_pad_id, X_PRESSED(XINPUT_GAMEPAD_RIGHT_THUMB));
        
        util_set_pad_state(PAD_BACK, m_pad_id, X_PRESSED(XINPUT_GAMEPAD_BACK));
        util_set_pad_state(PAD_START, m_pad_id, X_PRESSED(XINPUT_GAMEPAD_START));
        
        util_set_pad_state(PAD_X, m_pad_id, X_PRESSED(XINPUT_GAMEPAD_X));
        util_set_pad_state(PAD_Y, m_pad_id, X_PRESSED(XINPUT_GAMEPAD_Y));
        util_set_pad_state(PAD_A, m_pad_id, X_PRESSED(XINPUT_GAMEPAD_A));
        util_set_pad_state(PAD_B, m_pad_id, X_PRESSED(XINPUT_GAMEPAD_B));
    
        util_set_pad_state(PAD_LB, m_pad_id, X_PRESSED(XINPUT_GAMEPAD_LEFT_SHOULDER));
        util_set_pad_state(PAD_LT, m_pad_id, m_xinput.Gamepad.bLeftTrigger > 20);

        util_set_pad_state(PAD_RB, m_pad_id, X_PRESSED(XINPUT_GAMEPAD_RIGHT_SHOULDER));
        util_set_pad_state(PAD_RT, m_pad_id, m_xinput.Gamepad.bRightTrigger > 20);
    
        util_set_pad_state(PAD_DPAD_UP, m_pad_id, X_PRESSED(XINPUT_GAMEPAD_DPAD_UP));
        util_set_pad_state(PAD_DPAD_DOWN, m_pad_id, X_PRESSED(XINPUT_GAMEPAD_DPAD_DOWN));
        util_set_pad_state(PAD_DPAD_LEFT, m_pad_id, X_PRESSED(XINPUT_GAMEPAD_DPAD_LEFT));
        util_set_pad_state(PAD_DPAD_RIGHT, m_pad_id, X_PRESSED(XINPUT_GAMEPAD_DPAD_RIGHT));
    }
private:
    XINPUT_STATE m_xinput;
    bool m_valid = false;
    int8_t m_id = -1;
};

void update_gamepads(void);
#endif // HAVE_XINPUT

void start_pad_hook(void);
#ifdef LINUX
void * hook_method(void *);
#endif
void end_pad_hook(void);

void init_pad_devices(void);

#endif // GAMEPAD_HOOK_HPP