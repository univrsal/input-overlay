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
#include <linux/joystick.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/select.h>
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
        if (m_device_id)
            close(m_device_id);
        m_device_id = -1;
    }

    void load()
    {
        unload();
        m_device_id = open(m_path.c_str(), O_RDONLY);
        if (m_device_id >= 0) {
            /* Make this descriptor non blocking */
            int flags = fcntl(m_device_id, F_GETFL, 0);
            fcntl(m_device_id, F_SETFL, flags | O_NONBLOCK);
            blog(LOG_DEBUG, "[input-overlay] Pad %s present", m_path.c_str());
        }
    }

    void set_key(bool state, int id)
    {
        if (id < PAD_BUTTON_COUNT)
            m_keys[id] = state;
    }

    bool valid()
    { return m_device_id != -1; }

    void set_path(std::string path)
    {
        m_path = path;
        load();
    }

    int dev()
    { return m_device_id; }

    bool key_state(uint8_t id)
    {
        if (id < PAD_BUTTON_COUNT)
            return m_keys[id];
        return false;
    }

    int read_event()
    {
        ssize_t bytes;
        bytes = read(m_device_id, &m_event, sizeof(m_event));
        return bytes;
    }

    js_event* event() { return &m_event; }

    const std::string& path() { return m_path; }

    float l_x, l_y, r_x, r_y;

private:
    int m_device_id = -1;
    js_event m_event;
    std::string m_path;
    bool m_keys[PAD_BUTTON_COUNT];

};

extern GamepadState pad_states[PAD_COUNT]; // Only monitor four gamepads, I mean who even has more than four friends
extern volatile bool gamepad_hook_state;

void start_pad_hook(void);

void* hook_method(void*);

void end_pad_hook(void);

void init_pad_devices(void);

#endif // LINUX
#endif // GAMEPAD_HOOK_HPP
