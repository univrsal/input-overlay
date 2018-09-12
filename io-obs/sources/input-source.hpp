/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <obs-module.h>
#include <string>
#include <algorithm>
#include <clocale>
#include <locale>
#include <uiohook.h>
#include <list>

#include "../../ccl/ccl.hpp"
#include "../util/overlay.hpp"
#include "../util/util.hpp"

extern "C" {
#include <graphics/image-file.h>
}

#include "../hook/hook-helper.hpp"

#include "../hook/gamepad-hook.hpp"

namespace sources
{
    class input_source
    {
    public:
        obs_source_t* m_source = nullptr;
        uint32_t cx = 0;
        uint32_t cy = 0;

        /* Settings value */
        bool m_is_controller = false;
        uint16_t m_l_dz = 0, m_r_dz = 0; /* Analog stick deadzones */
        uint8_t m_pad_id = 0;
        bool m_monitor_use_center = false;
        int32_t m_monitor_h = 0, m_monitor_v = 0;
        uint8_t m_mouse_dead_zone = 0;
        std::string m_image_file;
        std::string m_layout_file;

        float m_old_angle = 0.f; /* For drawing the mouse arrow*/
        std::unique_ptr<overlay> m_overlay;

        input_source(obs_source_t* source_, obs_data_t* settings) :
            m_source(source_)
        {
            m_overlay = std::make_unique<overlay>();
            obs_source_update(m_source, settings);
        }

        ~input_source()
        {
        }


        void load_overlay() const;

        inline void update(obs_data_t* settings);
        inline void tick(float seconds);
        inline void render(gs_effect_t* effect);
    };

    static bool is_controller_changed(obs_properties_t* props, obs_property_t* p, obs_data_t* s);

    static bool is_mouse_changed(obs_properties_t* props, obs_property_t* p, obs_data_t* s);

    static bool use_monitor_center_changed(obs_properties_t* props, obs_property_t* p, obs_data_t* s);

    // For registering
    static obs_properties_t* get_properties_for_overlay(void* data);

    void register_overlay_source();
}
