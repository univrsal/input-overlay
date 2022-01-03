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

#include "input_source.hpp"
#include "../hook/gamepad_hook_helper.hpp"
#include "../util/lang.h"
#include "../util/obs_util.hpp"
#include "../util/settings.h"
#include "../util/config.hpp"
#include "../network/io_server.hpp"
#include "../network/remote_connection.hpp"
#include <QFile>
#include <obs-frontend-api.h>

namespace sources {
bool overlay_settings::use_local_input()
{
    return selected_source.empty() || selected_source == T_LOCAL_SOURCE;
}

input_source::input_source(obs_source_t *source, obs_data_t *settings) : m_source(source)
{
    m_overlay = std::make_unique<overlay>(&m_settings);
    obs_source_update(m_source, settings);
    m_settings.image_file = obs_data_get_string(settings, S_OVERLAY_FILE);
    m_settings.layout_file = obs_data_get_string(settings, S_LAYOUT_FILE);
    m_overlay->load();

    // Fix sources that used to use a remote connection but can't now because
    // remote connections are disabled
    if (!io_config::enable_remote_connections) {
        m_settings.selected_source = T_LOCAL_SOURCE;
        obs_data_set_string(settings, S_INPUT_SOURCE, T_LOCAL_SOURCE);
    }
}

input_source::~input_source() = default;

inline void input_source::update(obs_data_t *settings)
{
    m_settings.selected_source = obs_data_get_string(settings, S_INPUT_SOURCE);

    m_settings.gamepad_id = obs_data_get_string(settings, S_CONTROLLER_ID);
    if (m_settings.use_local_input() && libgamepad::hook_instance) {
        libgamepad::hook_instance->get_mutex()->lock();
        m_settings.gamepad = libgamepad::hook_instance->get_device_by_id(m_settings.gamepad_id);
        libgamepad::hook_instance->get_mutex()->unlock();
    } else if (io_config::enable_remote_connections) {
        std::lock_guard<std::mutex> lock(network::mutex);
        m_settings.gamepad =
            network::server_instance->get_client_device_by_id(m_settings.selected_source, m_settings.gamepad_id);
    }

    m_settings.mouse_sens = obs_data_get_int(settings, S_MOUSE_SENS);

    if ((m_settings.use_center = obs_data_get_bool(settings, S_MONITOR_USE_CENTER))) {
        m_settings.monitor_h = obs_data_get_int(settings, S_MONITOR_H_CENTER);
        m_settings.monitor_w = obs_data_get_int(settings, S_MONITOR_V_CENTER);
        m_settings.mouse_deadzone = obs_data_get_int(settings, S_MOUSE_DEAD_ZONE);
    }
}

inline void input_source::tick(float seconds)
{
    if (m_overlay->is_loaded()) {
        m_overlay->refresh_data();
        m_overlay->tick(seconds);
    }

    // If we don't have a gamepad check periodically to see if it has been connected
    if (m_settings.layout_flags & OF_GAMEPAD) {
        if (m_settings.gamepad && !m_settings.gamepad->is_valid()) {
            m_settings.gamepad = nullptr; // don't hold onto invalid references
        } else {
            m_settings.gamepad_check_timer += seconds;
            if (m_settings.gamepad_check_timer >= 1) {
                if (m_settings.use_local_input() && libgamepad::hook_instance) {
                    libgamepad::hook_instance->get_mutex()->lock();
                    m_settings.gamepad = libgamepad::hook_instance->get_device_by_id(m_settings.gamepad_id);
                    libgamepad::hook_instance->get_mutex()->unlock();
                } else if (network::network_flag) {
                    std::lock_guard<std::mutex> lock(network::mutex);
                    m_settings.gamepad = network::server_instance->get_client_device_by_id(m_settings.selected_source,
                                                                                           m_settings.gamepad_id);
                }
                m_settings.gamepad_check_timer = 0.0f;
            }
        }
    }
}

inline void input_source::render(gs_effect_t *effect) const
{
    if (!m_overlay->get_texture() || !m_overlay->get_texture()->texture)
        return;

    if (m_settings.layout_file.empty() || !m_overlay->is_loaded()) {
        gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), m_overlay->get_texture()->texture);
        gs_draw_sprite(m_overlay->get_texture()->texture, 0, cx, cy);
    } else {
        m_overlay->draw(effect);
    }
}

bool use_monitor_center_changed(obs_properties_t *props, obs_property_t *, obs_data_t *data)
{
    const auto use_center = obs_data_get_bool(data, S_MONITOR_USE_CENTER);
    obs_property_set_visible(GET_PROPS(S_MONITOR_H_CENTER), use_center);
    obs_property_set_visible(GET_PROPS(S_MONITOR_V_CENTER), use_center);
    return true;
}

bool reload_pads(obs_properties_t *props, obs_property_t *property, void *data)
{
    auto *src = static_cast<input_source *>(data);
    obs_property_list_clear(property);

    if (src->m_settings.use_local_input() && libgamepad::hook_instance) {
        libgamepad::hook_instance->get_mutex()->lock();
        for (const auto &pad : libgamepad::hook_instance->get_devices())
            obs_property_list_add_string(property, pad->get_id().c_str(), pad->get_id().c_str());
        libgamepad::hook_instance->get_mutex()->unlock();
    } else if (io_config::enable_remote_connections) {
        // Add remote gamepads
        std::lock_guard<std::mutex> lock(network::mutex);
        auto client = network::server_instance->get_client(src->m_settings.selected_source);
        if (client) {
            for (const auto &pad : client->gamepads())
                obs_property_list_add_string(property, pad.second->get_id().c_str(), pad.second->get_id().c_str());
        }
    }

    return true;
}

bool file_changed(void *d, obs_properties_t *props, obs_property_t *, obs_data_t *data)
{
    auto *src = static_cast<input_source *>(d);
    const auto *config = obs_data_get_string(data, S_LAYOUT_FILE);
    auto old_image_file = src->m_settings.image_file;
    src->m_settings.image_file = obs_data_get_string(data, S_OVERLAY_FILE);

    /* Only reload config file if path changed */
    if (src->m_settings.layout_file != config || src->m_settings.image_file != old_image_file) {
        src->m_settings.layout_file = config;
        if (!src->m_overlay->load()) {
            src->m_settings.layout_flags = 0;
        }
    }

    auto const &flags = src->m_settings.layout_flags;
    obs_property_set_visible(GET_PROPS(S_CONTROLLER_L_DEAD_ZONE), flags & OF_LEFT_STICK);
    obs_property_set_visible(GET_PROPS(S_CONTROLLER_R_DEAD_ZONE), flags & OF_RIGHT_STICK);
    obs_property_set_visible(GET_PROPS(S_CONTROLLER_ID),
                             flags & OF_GAMEPAD || (flags & OF_LEFT_STICK || flags & OF_RIGHT_STICK));
    obs_property_set_visible(GET_PROPS(S_MOUSE_SENS), flags & OF_MOUSE);
    obs_property_set_visible(GET_PROPS(S_MONITOR_USE_CENTER), flags & OF_MOUSE);
    obs_property_set_visible(GET_PROPS(S_MOUSE_DEAD_ZONE), flags & OF_MOUSE);
    obs_property_set_visible(GET_PROPS(S_RELOAD_PAD_DEVICES), flags & OF_GAMEPAD);
    reload_pads(nullptr, GET_PROPS(S_CONTROLLER_ID), src);

    return true;
}

bool reload_connections(obs_properties_t *, obs_property_t *property, void *)
{
    std::lock_guard<std::mutex> lock(network::mutex);
    network::server_instance->get_clients(property, network::local_input);
    return true;
}

obs_properties_t *get_properties_for_overlay(void *data)
{
    auto *src = static_cast<input_source *>(data);

    QString img_path, layout_path;
    auto *const props = obs_properties_create();
    const int flags = src->m_settings.layout_flags;

    const auto filter_img = util_file_filter(T_FILTER_IMAGE_FILES, "*.jpg *.png *.bmp");
    const auto filter_text = util_file_filter(T_FILTER_TEXT_FILES, "*.json");

    /* Config and texture file path */
    auto *texture = obs_properties_add_path(props, S_OVERLAY_FILE, T_TEXTURE_FILE, OBS_PATH_FILE,
                                            qt_to_utf8(filter_img), qt_to_utf8(img_path));
    auto *cfg = obs_properties_add_path(props, S_LAYOUT_FILE, T_LAYOUT_FILE, OBS_PATH_FILE, qt_to_utf8(filter_text),
                                        qt_to_utf8(layout_path));

    obs_property_set_modified_callback2(cfg, file_changed, data);
    obs_property_set_modified_callback2(texture, file_changed, data);

    /* If enabled add dropdown to select input source */
    if (CGET_BOOL(S_REMOTE)) {
        auto *list = obs_properties_add_list(props, S_INPUT_SOURCE, T_INPUT_SOURCE, OBS_COMBO_TYPE_EDITABLE,
                                             OBS_COMBO_FORMAT_STRING);
        obs_properties_add_button(props, S_RELOAD_CONNECTIONS, T_RELOAD_CONNECTIONS, reload_connections);
        if (network::network_flag) {
            network::server_instance->get_clients(list, network::local_input);
        }
    }
    /* Mouse stuff */
    obs_properties_add_int_slider(props, S_MOUSE_SENS, T_MOUSE_SENS, 1, 500, 1);

    const auto use_center = obs_properties_add_bool(props, S_MONITOR_USE_CENTER, T_MONITOR_USE_CENTER);
    obs_property_set_modified_callback(use_center, use_monitor_center_changed);

    obs_properties_add_int(props, S_MONITOR_H_CENTER, T_MONITOR_H_CENTER, -9999, 9999, 1);
    obs_properties_add_int(props, S_MONITOR_V_CENTER, T_MONITOR_V_CENTER, -9999, 9999, 1);
    obs_properties_add_int_slider(props, S_MOUSE_DEAD_ZONE, T_MOUSE_DEAD_ZONE, 0, 500, 1);

    /* Gamepad stuff */
    obs_property_set_visible(obs_properties_add_list(props, S_CONTROLLER_ID, T_CONTROLLER_ID, OBS_COMBO_TYPE_EDITABLE,
                                                     OBS_COMBO_FORMAT_STRING),
                             false);

    auto *btn = obs_properties_add_button2(props, S_RELOAD_PAD_DEVICES, T_RELOAD_PAD_DEVICES, reload_pads, src);
    obs_property_set_visible(btn, false);

    obs_property_set_visible(GET_PROPS(S_CONTROLLER_L_DEAD_ZONE), flags & OF_LEFT_STICK);
    obs_property_set_visible(GET_PROPS(S_CONTROLLER_R_DEAD_ZONE), flags & OF_RIGHT_STICK);
    obs_property_set_visible(GET_PROPS(S_CONTROLLER_ID),
                             flags & OF_GAMEPAD || (flags & OF_LEFT_STICK || flags & OF_RIGHT_STICK));
    obs_property_set_visible(GET_PROPS(S_MOUSE_SENS), flags & OF_MOUSE);
    obs_property_set_visible(GET_PROPS(S_MONITOR_USE_CENTER), flags & OF_MOUSE);
    obs_property_set_visible(GET_PROPS(S_MOUSE_DEAD_ZONE), flags & OF_MOUSE);
    obs_property_set_visible(GET_PROPS(S_RELOAD_PAD_DEVICES), flags & OF_GAMEPAD);
    reload_pads(nullptr, GET_PROPS(S_CONTROLLER_ID), src);
    return props;
}

void register_overlay_source()
{
    /* Input Overlay */
    obs_source_info si = {};
    si.id = "input-overlay";
    si.type = OBS_SOURCE_TYPE_INPUT;
    si.output_flags = OBS_SOURCE_VIDEO;
    si.get_properties = get_properties_for_overlay;
    si.icon_type = OBS_ICON_TYPE_GAME_CAPTURE;
    si.get_name = [](void *) { return obs_module_text("InputOverlay"); };
    si.create = [](obs_data_t *settings, obs_source_t *source) {
        return static_cast<void *>(new input_source(source, settings));
    };
    si.destroy = [](void *data) { delete static_cast<input_source *>(data); };
    si.get_width = [](void *data) { return static_cast<input_source *>(data)->m_settings.cx; };
    si.get_height = [](void *data) { return static_cast<input_source *>(data)->m_settings.cy; };
    si.get_defaults = [](obs_data_t *settings) { UNUSED_PARAMETER(settings); };
    si.update = [](void *data, obs_data_t *settings) { static_cast<input_source *>(data)->update(settings); };
    si.video_tick = [](void *data, float seconds) { static_cast<input_source *>(data)->tick(seconds); };
    si.video_render = [](void *data, gs_effect_t *effect) { static_cast<input_source *>(data)->render(effect); };
    obs_register_source(&si);
}
}
