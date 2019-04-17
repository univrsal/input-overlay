/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#ifndef GAMEPAD_HOOK_HPP
#define GAMEPAD_HOOK_HPP

#ifdef LINUX

#include "../util/util.hpp"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <malloc.h>

#define PAD_COUNT 4

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
        if (m_device_file) {
            void* tmp = malloc(8 * 12 * sizeof(char));
            fread(tmp, sizeof(char) * 8 * 12, 1, m_device_file);
            free(tmp);
        }
    }

    void set_key(bool state, int id)
    {
        if (id < PAD_BUTTON_COUNT)
            m_keys[id] = state;
    }

    bool valid()
    { return m_device_file != NULL; }

    void set_path(std::string path)
    {
        m_path = path;
        load();
    }

    FILE* dev()
    { return m_device_file; }

    float l_x, l_y, r_x, r_y;

    bool key_state(uint8_t id)
    {
        if (id < PAD_BUTTON_COUNT)
            return m_keys[id];
        return false;
    }

private:
    FILE* m_device_file = nullptr;
    std::string m_path;
    bool m_keys[PAD_BUTTON_COUNT];

};

extern GamepadState pad_states[PAD_COUNT]; // Only monitor four gamepads, I mean who even has more than four friends
extern bool gamepad_hook_state;

void start_pad_hook(void);

void* hook_method(void*);

void end_pad_hook(void);

void init_pad_devices(void);

#endif // LINUX
#endif // GAMEPAD_HOOK_HPP