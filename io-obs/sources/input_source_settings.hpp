/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <obs-module.h>
#include <string>

namespace settings
{
    class overlay_settings
    {

    private:
        bool m_has_mouse = false;
        bool m_has_gamepad = false;
        bool m_use_monitor_center = false;

        int32_t m_monitor_h = 0, m_monitor_v = 0;
        uint8_t m_mouse_dead_zone = 0;
        uint16_t m_left_dz = 0, m_right_dz = 0; /* Analog stick dead zones */
        uint8_t m_pad_id = 0;

        std::string m_image_file;
        std::string m_layout_file;
    };

    
}
