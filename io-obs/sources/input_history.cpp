/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#include "input_history.hpp"
#include "../util/element/element_button.hpp"
#include "graphics/matrix4.h"
#include "util/history/history_icons.hpp"
#include "util/history/key_names.hpp"
#include "util/history/input_queue.hpp"
#include "util/config-file.h"
#include "network/io_server.hpp"
#include <iomanip>
#include <obs-frontend-api.h>
#include <util/config.hpp>

namespace sources
{
    input_history_source::input_history_source(obs_source_t* source_, obs_data_t* settings)
    {
        m_settings.source = source_;
        m_settings.settings = settings;
        m_settings.queue = new input_queue(&m_settings);
        update(settings);
    }

    input_history_source::~input_history_source()
    {
        delete m_settings.queue;
    }

    void input_history_source::clear_history() const
    {
        if (m_settings.queue)
            m_settings.queue->clear();
    }

    inline void input_history_source::update(obs_data_t* settings)
    {
        /* Get the input source */
        if (hook::data_initialized || network::network_flag) {
            uint8_t source_id = obs_data_get_int(settings, S_INPUT_SOURCE);
            if (source_id > 0)
                m_settings.data = network::server_instance->get_client(source_id)->get_data();
            else
                m_settings.data = hook::input_data;
        }

        SET_FLAG((int) history_flags::INCLUDE_MOUSE, obs_data_get_bool(settings, S_HISTORY_INCLUDE_MOUSE));
        SET_FLAG((int) history_flags::REPEAT_KEYS, obs_data_get_bool(settings, S_HISTORY_ENABLE_REPEAT_KEYS));
        SET_FLAG((int) history_flags::AUTO_CLEAR, obs_data_get_bool(settings, S_HISTORY_ENABLE_AUTO_CLEAR));
        SET_FLAG((int) history_flags::FIX_CUTTING, obs_data_get_bool(settings, S_HISTORY_FIX_CUTTING));
        SET_FLAG((int) history_flags::USE_FALLBACK, obs_data_get_bool(settings, S_HISTORY_USE_FALLBACK_NAME));
        SET_FLAG((int) history_flags::INCLUDE_PAD, obs_data_get_bool(settings, S_HISTORY_INCLUDE_PAD));

        m_settings.update_interval = obs_data_get_double(settings, S_HISTORY_INTERVAL);
        m_settings.auto_clear_interval = obs_data_get_double(settings, S_HISTORY_AUTO_CLEAR_INTERVAL);

        m_settings.key_name_path = obs_data_get_string(settings, S_HISTORY_KEY_NAME_PATH);
        m_settings.icon_cfg_path = obs_data_get_string(settings, S_HISTORY_KEY_ICON_CONFIG_PATH);
        m_settings.icon_path = obs_data_get_string(settings, S_HISTORY_KEY_ICON_PATH);

        m_settings.h_space = obs_data_get_int(settings, S_HISTORY_ICON_H_SPACE);
        m_settings.v_space = obs_data_get_int(settings, S_HISTORY_ICON_V_SPACE);

        m_settings.history_size = obs_data_get_int(settings, S_HISTORY_SIZE);
        m_settings.dir = history_direction(obs_data_get_int(settings, S_HISTORY_DIRECTION));

        if (!m_settings.key_name_path || strlen(m_settings.key_name_path) < 1)
            SET_FLAG((int) history_flags::CUSTOM_NAMES, false);
        else
            SET_FLAG((int) history_flags::CUSTOM_NAMES, true);

        if (GET_FLAG((int) history_flags::INCLUDE_PAD))
            m_settings.target_gamepad = static_cast<uint8_t>(obs_data_get_int(settings, S_CONTROLLER_ID));

        /* Order is important here */
        const auto new_mode = history_mode(obs_data_get_int(settings, S_HISTORY_MODE));
        m_settings.queue->update(new_mode); /* Apply new settings to queue */
        m_settings.mode = new_mode;
    }

    inline void input_history_source::tick(float seconds)
    {
        if (!obs_source_showing(m_settings.source))
            return;
        if (io_config::io_window_filters.input_blocked())
            return; /* No input collection if it's blocked */

        m_settings.queue->tick(seconds);

        if (GET_FLAG((int) history_flags::AUTO_CLEAR)) {
            m_clear_timer += seconds;
            if (m_settings.auto_clear_interval <= m_clear_timer) {
                m_clear_timer = 0.f;
                clear_history();
            }
        }

        m_collect_timer += seconds;
        if (m_collect_timer >= m_settings.update_interval) {
            m_collect_timer = 0.f;
            m_clear_timer = 0.f;
            /* Moves current input entry from collection into list
             * only if there's been any changes in input */
            if (m_last_input != m_settings.data->get_last_input()) {
                m_last_input = m_settings.data->get_last_input();
                m_settings.queue->swap();
            }
        } else {
            m_settings.queue->collect_input();
        }
    }

    inline void input_history_source::render(gs_effect_t* effect) const
    {
        m_settings.queue->render(effect);
    }

    bool clear_history(obs_properties_t* props, obs_property_t* property, void* data)
    {
        UNUSED_PARAMETER(props);
        UNUSED_PARAMETER(property);
        auto* source = reinterpret_cast<input_history_source*>(data);
        source->clear_history();
        return true;
    }

    bool include_pad_changed(obs_properties* props, obs_property_t* p, obs_data_t* s)
    {
        UNUSED_PARAMETER(p);
        const auto id = obs_properties_get(props, S_CONTROLLER_ID);
        obs_property_set_visible(id, obs_data_get_bool(s, S_HISTORY_INCLUDE_PAD));
        return true;
    }

    /* Hides/shows all necessary properties of child text source */
    bool toggle_font_settings(obs_properties_t* props, obs_property_t* p, obs_data_t* data)
    {
        UNUSED_PARAMETER(p);
        const auto show = obs_data_get_bool(data, S_HISTORY_SHOW_FONT);
        auto prop = obs_properties_first(props);
        auto use_gradient = obs_data_get_bool(data, "gradient");
        auto use_outline = obs_data_get_bool(data, "outline");

        for (; prop; obs_property_next(&prop)) {
            auto name = obs_property_name(prop);

            if (!strcmp(name, "read_from_file") || !strcmp(name, "text") || !strcmp(name, "file") ||
                !strcmp(name, "vertical") || strstr(name, "align") || strstr(name, "extents") ||
                strstr(name, "chatlog") ||
                (!use_gradient && strstr(name, "gradient") && strlen(name) > strlen("gradient")) ||
                (!use_outline && strstr(name, "outline") &&
                 strlen(name) > strlen("outline"))) {  /* do not show unnecessary properties */
                obs_property_set_visible(prop, false);
            } else if (!strstr(name, "io")) /* All input-history values start with 'io.' ->
                                                    if the value does not contain 'io' it's part
                                                    of the text source properties */
            {
                obs_property_set_visible(prop, show);
            }
        }
        return true;
    }

#define TEXT_VIS(S)     (obs_property_set_visible(S, mode == (int) history_mode::TEXT))
#define ICON_VIS(S)     (obs_property_set_visible(S, mode == (int) history_mode::ICONS))

    bool mode_changed(obs_properties_t* props, obs_property_t* p, obs_data_t* s)
    {
        const auto mode = obs_data_get_int(s, S_HISTORY_MODE);

        TEXT_VIS(GET_PROPS(S_HISTORY_SHOW_FONT));
        ICON_VIS(GET_PROPS(S_HISTORY_KEY_ICON_CONFIG_PATH));
        ICON_VIS(GET_PROPS(S_HISTORY_KEY_ICON_PATH));
        ICON_VIS(GET_PROPS(S_HISTORY_ICON_V_SPACE));
        ICON_VIS(GET_PROPS(S_HISTORY_ICON_H_SPACE));

        toggle_font_settings(props, p, s); /* Update font property visibility */
        return true;
    }

    bool reload_connections(obs_properties_t* props, obs_property_t* property, void* data)
    {
        UNUSED_PARAMETER(property);
        UNUSED_PARAMETER(data);
        const auto connection_list = obs_properties_get(props, S_INPUT_SOURCE);
        if (connection_list)
            network::server_instance->get_clients(connection_list, network::local_input);

        return true;
    }

    obs_properties_t* get_properties_for_history(void* data)
    {
        const auto s = reinterpret_cast<input_history_source*>(data);
        obs_properties_t* props = nullptr;

        if (s->m_settings.mode != history_mode::ICONS)
            props = obs_source_properties(s->m_settings.queue->get_fade_in()); /* Reuse text properties */
        else
            props = obs_properties_create();

        /* Hide font properties by default */
        auto prop = obs_properties_first(props);
        for (; prop; obs_property_next(&prop))
            obs_property_set_visible(prop, false);

        /* Add option to toggle font options (Less clutter) */
        const auto font_settings = obs_properties_add_bool(props, S_HISTORY_SHOW_FONT, T_HISTORY_SHOW_FONT);
        obs_property_set_modified_callback(font_settings, toggle_font_settings);

        /* If enabled add dropdown to select input source */
        if (io_config::remote) {
            auto list = obs_properties_add_list(props, S_INPUT_SOURCE, T_INPUT_SOURCE, OBS_COMBO_TYPE_LIST,
                                                OBS_COMBO_FORMAT_INT);
            obs_properties_add_button(props, S_RELOAD_CONNECTIONS, T_RELOAD_CONNECTIONS, reload_connections);
            if (network::network_flag) {
                network::server_instance->get_clients(list, network::local_input);
            }
        }

        const auto mode_list = obs_properties_add_list(props, S_HISTORY_MODE, T_HISTORY_MODE, OBS_COMBO_TYPE_LIST,
                                                       OBS_COMBO_FORMAT_INT);

        obs_property_list_add_int(mode_list, T_HISTORY_MODE_TEXT, (int) history_mode::TEXT);
        obs_property_list_add_int(mode_list, T_HISTORY_MODE_ICON, (int) history_mode::ICONS);
        obs_property_set_modified_callback(mode_list, mode_changed);

        /* Key name, icon and config file */
        auto filter_img = util_file_filter(T_FILTER_IMAGE_FILES, "*.jpg *.png *.bmp");
        auto filter_text = util_file_filter(T_FILTER_TEXT_FILES, "*.ini");
        std::string key_names_path, key_icon_path, key_icon_config_path;

        if (s->m_settings.key_name_path) {
            key_names_path = s->m_settings.key_name_path;
            util_format_path(key_names_path);
        }

        if (s->m_settings.icon_path) {
            key_icon_path = s->m_settings.icon_path;
            util_format_path(key_icon_path);
        }

        if (s->m_settings.icon_cfg_path) {
            key_icon_config_path = s->m_settings.icon_cfg_path;
            util_format_path(key_icon_config_path);
        }

        /* Icon mode properties */
        obs_properties_add_path(props, S_HISTORY_KEY_ICON_PATH, T_HISTORY_KEY_ICON_PATH, OBS_PATH_FILE,
                                filter_img.c_str(), key_icon_path.c_str());

        obs_properties_add_path(props, S_HISTORY_KEY_ICON_CONFIG_PATH, T_HISTORY_KEY_ICON_CONFIG_PATH, OBS_PATH_FILE,
                                filter_text.c_str(), key_icon_config_path.c_str());

        obs_properties_add_int(props, S_HISTORY_ICON_H_SPACE, T_HISTORY_ICON_H_SPACE, -1000, 1000, 1);
        obs_properties_add_int(props, S_HISTORY_ICON_V_SPACE, T_HISTORY_ICON_V_SPACE, -1000, 1000, 1);

        /* Text mode properties*/
        obs_properties_add_path(props, S_HISTORY_KEY_NAME_PATH, T_HISTORY_KEY_NAME_PATH, OBS_PATH_FILE,
                                filter_text.c_str(), key_names_path.c_str());
        obs_properties_add_bool(props, S_HISTORY_USE_FALLBACK_NAME, T_HISTORY_USE_FALLBACK_NAMES);

        /* Entry flow direction */
        const auto icon_dir_list = obs_properties_add_list(props, S_HISTORY_DIRECTION, T_HISTORY_DIRECTION,
                                                           OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);

        obs_property_list_add_int(icon_dir_list, T_HISTORY_DIRECTION_DOWN, 0);
        obs_property_list_add_int(icon_dir_list, T_HISTORY_DIRECTION_UP, 1);
#ifdef WIN32 /* Freetype doesn't offer vertical text direction */
        obs_property_list_add_int(icon_dir_list, T_HISTORY_DIRECTION_LEFT, 2);
        obs_property_list_add_int(icon_dir_list, T_HISTORY_DIRECTION_RIGHT, 3);
#endif
        /* gamepad */
        if (io_config::gamepad) {
            const auto include_pad = obs_properties_add_bool(props, S_HISTORY_INCLUDE_PAD, T_HISTORY_INCLUDE_PAD);
            obs_property_set_modified_callback(include_pad, include_pad_changed);
            obs_properties_add_int(props, S_CONTROLLER_ID, T_CONTROLLER_ID, 0, 3, 1);
        }

        /* Auto clear */
        obs_properties_add_bool(props, S_HISTORY_ENABLE_AUTO_CLEAR, T_HISTORY_ENABLE_AUTO_CLEAR);
        obs_properties_add_float_slider(props, S_HISTORY_AUTO_CLEAR_INTERVAL, T_HISTORY_AUTO_CLEAR_INTERVAL, 0.1, 10,
                                        0.1);

        obs_properties_add_button(props, S_HISTORY_CLEAR_HISTORY, T_HISTORY_CLEAR_HISTORY, clear_history);

        /* Misc */
        obs_properties_add_int(props, S_HISTORY_SIZE, T_HISTORY_HISTORY_SIZE, 1, MAX_HISTORY_SIZE, 1);
        obs_properties_add_bool(props, S_HISTORY_FIX_CUTTING, T_HISTORY_FIX_CUTTING);
        obs_properties_add_bool(props, S_HISTORY_INCLUDE_MOUSE, T_HISTORY_INCLUDE_MOUSE);

        obs_properties_add_float_slider(props, S_HISTORY_INTERVAL, T_HISTORY_INTERVAL, 0.1, 10, 0.1);

        obs_properties_add_bool(props, S_HISTORY_ENABLE_REPEAT_KEYS, T_HISTORY_ENABLE_REPEAT_KEYS);

        return props;
    }

    void register_history()
    {
        obs_source_info si = {};
        si.id = "input-history";
        si.type = OBS_SOURCE_TYPE_INPUT;
        si.output_flags = OBS_SOURCE_VIDEO;
        si.get_properties = get_properties_for_history;

        si.get_name = [](void*)
        { return obs_module_text("InputHistory"); };
        si.create = [](obs_data_t* settings, obs_source_t* source)
        {
            return static_cast<void*>(new input_history_source(source, settings));
        };
        si.destroy = [](void* data)
        {
            delete reinterpret_cast<input_history_source*>(data);
        };
        si.get_width = [](void* data)
        {
            return reinterpret_cast<input_history_source*>(data)->get_width();
        };
        si.get_height = [](void* data)
        {
            return reinterpret_cast<input_history_source*>(data)->get_height();
        };

        si.get_defaults = [](obs_data_t* settings)
        {
            obs_data_set_default_int(settings, S_HISTORY_SIZE, 1);
            obs_data_set_default_int(settings, S_HISTORY_INTERVAL, 2);
            obs_data_set_default_int(settings, S_HISTORY_AUTO_CLEAR_INTERVAL, 2);
        };

        si.update = [](void* data, obs_data_t* settings)
        {
            reinterpret_cast<input_history_source*>(data)->update(settings);
        };
        si.video_tick = [](void* data, float seconds)
        {
            reinterpret_cast<input_history_source*>(data)->tick(seconds);
        };
        si.video_render = [](void* data, gs_effect_t* effect)
        {
            reinterpret_cast<input_history_source*>(data)->render(effect);
        };
        obs_register_source(&si);
    }
}
