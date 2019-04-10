/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "../util/overlay.hpp"
#include <obs-module.h>
#include <string>
#include <uiohook.h>

extern "C" {
#include <graphics/image-file.h>
}

typedef struct obs_data obs_data_t;

namespace sources
{
    class overlay_settings
    {
    public:
        std::string image_file;
        std::string layout_file;

        uint32_t cx = 0, cy = 0;                /* Source width/height */
        bool use_center = false;                /* true if monitor center is used for mouse movement */
        uint32_t monitor_w = 0, monitor_h = 0;
        uint8_t mouse_deadzone = 0;
        uint16_t mouse_sens = 0;                /* mouse_delta / mouse_sens = mouse movement */
        uint8_t gamepad = 0;                    /* selected gamepad id */

#ifdef _WIN32
        float left_dz = 0.f, right_dz = 0.f;    /* Windows gamepad analog sticks deadzone  (in %)*/
#endif
        uint8_t selected_source = 0;            /* 0 = Local input */
        uint8_t layout_flags = 0;               /* See overlay_flags in layout_constants.hpp */
        obs_data_t* data = nullptr;             /* Pointer to source property data */
    };

    class input_source
    {
    public:
        obs_source_t* m_source = nullptr;

        uint32_t cx = 0, cy = 0;
        std::unique_ptr<overlay> m_overlay{};
        overlay_settings m_settings;

        input_source(obs_source_t* source, obs_data_t* settings) : m_source(source)
        {
            m_overlay = std::make_unique<overlay>(&m_settings);
            m_settings.data = settings;
            obs_source_update(m_source, settings);
        }

        ~input_source() = default;

        inline void update(obs_data_t* settings);

        inline void tick(float seconds);

        inline void render(gs_effect_t* effect) const;
    };

    /* Event handlers */
    static bool use_monitor_center_changed(obs_properties_t* props, obs_property_t* p, obs_data_t* data);

    static bool reload_connections(obs_properties_t* props, obs_property_t* property, void* data);

    /* For registering */
    static obs_properties_t* get_properties_for_overlay(void* data);

    void register_overlay_source();
}
