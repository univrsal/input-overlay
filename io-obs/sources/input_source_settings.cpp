/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "input_source_settings.hpp"
#include "../util/util.hpp"

void settings::overlay_settings::update(obs_data_t *settings)
{
    m_image_file = obs_data_get_string(settings, S_OVERLAY_FILE);
    m_layout_file = obs_data_get_string(settings, S_LAYOUT_FILE);

    m_use_monitor_center = obs_data_get_bool(settings, S_MONITOR_USE_CENTER);
    m_mouse_dead_zone = static_cast<uint8_t>(obs_data_get_int(settings, S_MOUSE_DEAD_ZONE));

    m_monitor_h = static_cast<uint32_t>(obs_data_get_int(settings, S_MONITOR_H_CENTER));
    m_monitor_v = static_cast<uint32_t>(obs_data_get_int(settings, S_MONITOR_V_CENTER));
}

bool settings::overlay_settings::invalid_paths() {
    return m_image_file.empty() || m_layout_file.empty();
}
