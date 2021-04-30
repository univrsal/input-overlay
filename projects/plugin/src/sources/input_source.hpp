/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2021 univrsal <uni@vrsal.de>.
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

#include "../util/overlay.hpp"
#include "../util/input_data.hpp"
#include <obs-module.h>
#include <string>

extern "C" {
#include <graphics/image-file.h>
}

typedef struct obs_data obs_data_t;

namespace sources {
class overlay_settings {
public:
    /* clang-format: off */
    std::string image_file;
    std::string layout_file;

    input_data data{};                        /* Copy of input data used for visualization          */
    uint32_t cx = 0, cy = 0;                  /* Source width/height                                */
    bool use_center = false;                  /* true if monitor center is used for mouse movement	*/
    uint32_t monitor_w = 0, monitor_h = 0;    /* Monitor size used for mouse movement               */
    uint8_t mouse_deadzone = 0;               /* Region in which to ignore mouse movements          */
    uint16_t mouse_sens = 0;                  /* mouse_delta / mouse_sens = mouse movement			*/
    std::shared_ptr<gamepad::device> gamepad; /* selected gamepad                                   */

    std::string selected_source;      /* Name of client or empty for local computer         */
    uint8_t layout_flags = 0;         /* See overlay_flags in layout_constants.hpp          */
    float gamepad_check_timer = 0.0f; /* Counter to check if selected game pad is connected */
    std::string gamepad_id;
    /* clang-format: on */
};

class input_source {
public:
    obs_source_t *m_source = nullptr;

    uint32_t cx = 0, cy = 0;
    std::unique_ptr<overlay> m_overlay{};
    overlay_settings m_settings;

    input_source(obs_source_t *source, obs_data_t *settings) : m_source(source)
    {
        m_overlay = std::make_unique<overlay>(&m_settings);
        obs_source_update(m_source, settings);
    }

    ~input_source();

    inline void update(obs_data_t *settings);

    inline void tick(float seconds);

    inline void render(gs_effect_t *effect) const;
};

/* Event handlers */
static bool use_monitor_center_changed(obs_properties_t *props, obs_property_t *p, obs_data_t *data);

static bool reload_connections(obs_properties_t *props, obs_property_t *property, void *data);

/* For registering */
static obs_properties_t *get_properties_for_overlay(void *data);

void register_overlay_source();
}
