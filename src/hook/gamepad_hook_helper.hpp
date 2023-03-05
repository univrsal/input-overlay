/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2022 univrsal <uni@vrsal.xyz>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#pragma once

#include <mutex>
#include <memory>
#include <vector>
#include <map>
#include <atomic>
#include <algorithm>
#include <array>
#include <string>
#include "SDL.h"

#include "sdl_gamepad.hpp"

struct input_data;

namespace gamepad_hook {
extern std::atomic<bool> state;

inline const char *controller_description(int index)
{

    switch (SDL_GameControllerTypeForIndex(index)) {
    case SDL_CONTROLLER_TYPE_AMAZON_LUNA:
        return "Amazon Luna Controller";
    case SDL_CONTROLLER_TYPE_GOOGLE_STADIA:
        return "Google Stadia Controller";
#if SDL_VERSION_ATLEAST(2, 24, 0)
    case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:
    case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT:
    case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_JOYCON_PAIR:
        return "Nintendo Switch Joy-Con";
#endif
    case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_PRO:
        return "Nintendo Switch Pro Controller";
    case SDL_CONTROLLER_TYPE_PS3:
        return "PS3 Controller";
    case SDL_CONTROLLER_TYPE_PS4:
        return "PS4 Controller";
    case SDL_CONTROLLER_TYPE_PS5:
        return "PS5 Controller";
    case SDL_CONTROLLER_TYPE_XBOX360:
        return "XBox 360 Controller";
    case SDL_CONTROLLER_TYPE_XBOXONE:
        return "XBox One Controller";
    case SDL_CONTROLLER_TYPE_VIRTUAL:
        return "Virtual Game Controller";
    default:
        return "Game Controller";
    }
}

class gamepads {
    std::mutex m_mutex{};
    std::map<int, std::shared_ptr<sdl_gamepad>> m_pads{};

    static const char *get_sensor_name(SDL_SensorType sensor)
    {
        switch (sensor) {
        case SDL_SENSOR_ACCEL:
            return "accelerometer";
#if SDL_VERSION_ATLEAST(2, 26, 0)
        case SDL_SENSOR_GYRO:
            return "gyro";
        case SDL_SENSOR_ACCEL_L:
            return "accelerometer (L)";
        case SDL_SENSOR_GYRO_L:
            return "gyro (L)";
        case SDL_SENSOR_ACCEL_R:
            return "accelerometer (R)";
        case SDL_SENSOR_GYRO_R:
            return "gyro (R)";
#endif
        default:
            return "UNKNOWN";
        }
    }

public:
    gamepads();

    auto &mutex() { return m_mutex; }
    auto &pads() { return m_pads; }

    void add_controller(int index, std::string const &desc)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pads[index] = std::make_shared<sdl_gamepad>(index, desc);
    }

    void remove_controller(int index)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_pads.find(index) != m_pads.end()) {
            m_pads[index]->invalidate();
            m_pads.erase(index);
        }
    }

    std::shared_ptr<sdl_gamepad> get_controller(int index)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_pads.find(index) != m_pads.end())
            return m_pads[index];
        return nullptr;
    }

    void event_loop();
};

void start();
void stop();

extern gamepads *local_gamepads;
}
