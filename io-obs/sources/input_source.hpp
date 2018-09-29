/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <obs-module.h>
#include <string>
#include <uiohook.h>
#include "../util/overlay.hpp"

extern "C" {
#include <graphics/image-file.h>
}

typedef struct obs_data obs_data_t;

namespace sources
{

    struct shared_settings
    {
        std::string image_file;
        std::string layout_file;
        uint32_t cx = 0, cy = 0;
        uint8_t gamepad = 0;
        /* TODO: Mouse config etc.*/
    };

    class input_source
    {
    public:
        obs_source_t* m_source = nullptr;
        uint32_t cx = 0, cy = 0;
        std::unique_ptr<overlay> m_overlay{};
        shared_settings m_settings;

        input_source(obs_source_t* source, obs_data_t* settings) :
            m_source(source)
        {
            m_overlay = std::make_unique<overlay>(&m_settings);
            obs_source_update(m_source, settings);

        }

        ~input_source() = default;

        inline void update(obs_data_t* settings);
        inline void tick(float seconds);
        inline void render(gs_effect_t* effect) const;
    };

    static bool use_monitor_center_changed(obs_properties_t* props, obs_property_t* p, obs_data_t* s);

    /* For registering */
    static obs_properties_t* get_properties_for_overlay(void* data);

    void register_overlay_source();
}
