/**
 * This file is part of input-overlay
 * which is licensed under the
 * MOZILLA PUBLIC LICENCE 2.0 - mozilla.org/en-US/MPL/2.0/
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include <iomanip>
#include "input_history.hpp"
#include "../util/element/element_button.hpp"
#include "graphics/matrix4.h"
#include "util/history/history_icons.hpp"
#include "util/history/key_names.hpp"
#include "util/history/input_queue.hpp"

namespace sources
{
    input_history_source::input_history_source(obs_source_t* source_, obs_data_t* settings)
    {
        m_settings.source = source_;
        m_settings.settings = settings;
        m_settings.queue = new input_queue(&m_settings);
        obs_source_update(source_, settings);
        load_text_source();
    }

    input_history_source::~input_history_source()
    {
        unload_text_source();
        unload_icons();
        unload_translation();
        unload_command_handler();
    }

    void input_history_source::load_text_source()
    {
#ifdef _WIN32
        m_settings.text_source = obs_source_create("text_gdiplus\0", "history-text-source",
            m_settings.settings, nullptr);
#else
        m_settings.text_source = obs_source_create("text_ft2_source\0", "history-text-source", m_settings.settings, nullptr);
#endif
        obs_source_add_active_child(m_settings.source, m_settings.text_source);
    }

    void input_history_source::load_icons()
    {
        if (m_settings.icons == nullptr)
            m_settings.icons = new history_icons();
        m_settings.icons->load_from_file(m_settings.icon_cfg_path, m_settings.icon_path);
    }

    void input_history_source::load_translation()
    {
        if (m_settings.names == nullptr)
            m_settings.names = new key_names();
        m_settings.names->load_from_file(m_settings.key_name_path);
    }

    void input_history_source::load_command_handler()
    {
        if (!m_command_handler)
            m_command_handler = new command_handler();
    }

    void input_history_source::unload_text_source()
    {
        
        obs_source_remove(m_settings.text_source);
        obs_source_release(m_settings.text_source);
        m_settings.text_source = nullptr;
    }

    void input_history_source::unload_icons()
    {
        if (m_settings.icons)
        {
            delete m_settings.icons;
            m_settings.icons = nullptr;
        }
    }

    void input_history_source::unload_translation()
    {
        if (m_settings.names)
        {
            delete m_settings.names;
            m_settings.names = nullptr;
        }
    }

    inline void input_history_source::unload_command_handler()
    {
        if (m_command_handler)
        {
            delete m_command_handler;
            m_command_handler = nullptr;
        }
    }

    void input_history_source::clear_history() const
    {
        if (m_settings.queue)
            m_settings.queue->clear();
    }

    inline void input_history_source::update(obs_data_t* settings)
    {
        obs_source_update(m_settings.text_source, settings);

        
        m_settings.mode = history_mode(obs_data_get_int(settings, S_HISTORY_MODE));

        SET_FLAG(FLAG_INCLUDE_MOUSE, obs_data_get_bool(settings,
            S_HISTORY_INCLUDE_MOUSE));
        SET_FLAG(FLAG_REPEAT_KEYS, obs_data_get_bool(settings,
            S_HISTORY_ENABLE_REPEAT_KEYS));
        SET_FLAG(FLAG_AUTO_CLEAR, obs_data_get_bool(settings,
            S_HISTORY_ENABLE_AUTO_CLEAR));
        SET_FLAG(FLAG_FIX_CUTTING, obs_data_get_bool(settings,
            S_HISTORY_FIX_CUTTING));
        SET_FLAG(FLAG_USE_FALLBACK, obs_data_get_bool(settings,
            S_HISTORY_USE_FALLBACK_NAME));
        SET_FLAG(FLAG_INCLUDE_PAD, obs_data_get_bool(settings,
            S_HISTORY_INCLUDE_PAD));

        m_settings.update_interval = obs_data_get_double(settings, S_HISTORY_INTERVAL);
        m_settings.auto_clear_interval = obs_data_get_int(settings,
            S_HISTORY_AUTO_CLEAR_INTERVAL);

        m_settings.key_name_path = obs_data_get_string(settings, S_HISTORY_KEY_NAME_PATH);
        m_settings.icon_cfg_path = obs_data_get_string(
            settings, S_HISTORY_KEY_ICON_CONFIG_PATH);
        m_settings.icon_path = obs_data_get_string(settings, S_HISTORY_KEY_ICON_PATH);

        m_settings.h_space = obs_data_get_int(settings, S_HISTORY_ICON_H_SPACE);
        m_settings.v_space = obs_data_get_int(settings, S_HISTORY_ICON_V_SPACE);

        m_settings.history_size = obs_data_get_int(settings, S_HISTORY_SIZE);
        m_settings.dir = history_direction(obs_data_get_int(
            settings, S_HISTORY_DIRECTION));

        if (m_settings.mode == MODE_COMMANDS)
            load_command_handler();
        else
            unload_command_handler();

        if (m_settings.mode == MODE_ICONS)
            unload_icons();
        else
            load_icons();

        if (!m_settings.key_name_path || strlen(m_settings.key_name_path) < 1)
        {
            SET_FLAG(FLAG_CUSTOM_NAMES, false);
            unload_translation();
        }
        else
        {
            SET_FLAG(FLAG_CUSTOM_NAMES, true);
            load_translation();
        }

        if (GET_FLAG(FLAG_INCLUDE_PAD))
            m_settings.target_gamepad = static_cast<uint8_t>(obs_data_get_int(
                settings, S_CONTROLLER_ID));
    }

    inline void input_history_source::tick(float seconds)
    {
        if (!obs_source_showing(m_settings.source))
            return;

        if (GET_FLAG(FLAG_AUTO_CLEAR))
        {
            m_clear_timer += seconds;
            if (m_settings.auto_clear_interval <= m_clear_timer)
            {
                m_clear_timer = 0.f;
                clear_history();
            }
        }

        if (GET_FLAG(FLAG_USE_MASKS))
        {
            
        }
        else
        {
            m_collect_timer += seconds;
            if (m_collect_timer >= m_settings.update_interval)
            {
                m_collect_timer = 0.f;
                /* Moves current input entry from collection into list */
                m_settings.queue->swap();
            }
            else
            {
                m_settings.queue->collect_input();
            }
        }
    }

    inline void input_history_source::render(gs_effect_t* effect) const
    {
        m_settings.queue->render(effect);
    }

    bool clear_history(obs_properties_t* props, obs_property_t* property,
        void* data)
    {
        auto* source = reinterpret_cast<input_history_source*>(data);
        source->clear_history();
        return true;
    }

#define TEXT_VIS(S)     (obs_property_set_visible(S, state_text))
#define ICON_VIS(S)     (obs_property_set_visible(S, !state_text))

    bool mode_changed(obs_properties_t* props, obs_property_t* p, obs_data_t* s)
    {
        const auto state_text = obs_data_get_int(s, S_HISTORY_MODE) == 0;

        TEXT_VIS(GET_PROPS(S_HISTORY_FONT));
        TEXT_VIS(GET_PROPS(S_HISTORY_FONT_COLOR));
        TEXT_VIS(GET_PROPS(S_HISTORY_OUTLINE));
        TEXT_VIS(GET_PROPS(S_HISTORY_OUTLINE_SIZE));
        TEXT_VIS(GET_PROPS(S_HISTORY_OUTLINE_COLOR));
        TEXT_VIS(GET_PROPS(S_HISTORY_OUTLINE_OPACITY));
        TEXT_VIS(GET_PROPS(S_HISTORY_FIX_CUTTING));
        TEXT_VIS(GET_PROPS(S_HISTORY_OPACITY));
        TEXT_VIS(GET_PROPS(S_HISTORY_KEY_NAME_PATH));
        TEXT_VIS(GET_PROPS(S_HISTORY_USE_FALLBACK_NAME));
        TEXT_VIS(GET_PROPS(S_HISTORY_COMMAND_MODE));

        ICON_VIS(GET_PROPS(S_HISTORY_KEY_ICON_CONFIG_PATH));
        ICON_VIS(GET_PROPS(S_HISTORY_KEY_ICON_PATH));
        ICON_VIS(GET_PROPS(S_HISTORY_ICON_V_SPACE));
        ICON_VIS(GET_PROPS(S_HISTORY_ICON_H_SPACE));

        return true;
    }

    bool include_pad_changed(obs_properties* props, obs_property_t* p,
        obs_data_t* s)
    {
        const auto id = obs_properties_get(props, S_CONTROLLER_ID);
        obs_property_set_visible(id, obs_data_get_bool(s, S_HISTORY_INCLUDE_PAD));
        return true;
    }

    obs_properties_t* get_properties_for_history(void* data)
    {
        const auto s = reinterpret_cast<input_history_source*>(data);
        const auto props = obs_properties_create();
        const auto mode_list = obs_properties_add_list(props, S_HISTORY_MODE,
            T_HISTORY_MODE,
            OBS_COMBO_TYPE_LIST,
            OBS_COMBO_FORMAT_INT);
        obs_property_list_add_int(mode_list, T_HISTORY_MODE_TEXT, 0);
        obs_property_list_add_int(mode_list, T_HISTORY_MODE_ICON, 1);
        obs_property_set_modified_callback(mode_list, mode_changed);

        /* Key name file */
        auto filter_img = util_file_filter(
            T_FILTER_IMAGE_FILES, "*.jpg *.png *.bmp");
        auto filter_text = util_file_filter(T_FILTER_TEXT_FILES, "*.ini");
        std::string key_names_path, key_icon_path, key_icon_config_path;


        if (s->m_settings.key_name_path)
        {
            key_names_path = s->m_settings.key_name_path;
            util_format_path(key_names_path);
        }

        if (s->m_settings.icon_path)
        {
            key_icon_path = s->m_settings.icon_path;
            util_format_path(key_icon_path);
        }

        if (s->m_settings.icon_cfg_path)
        {
            key_icon_config_path = s->m_settings.icon_cfg_path;
            util_format_path(key_icon_config_path);
        }
        
        /* Icon mode propterties */
        obs_properties_add_path(props, S_HISTORY_KEY_ICON_PATH,
            T_HISTORY_KEY_ICON_PATH, OBS_PATH_FILE,
            filter_img.c_str(), key_icon_path.c_str());

        obs_properties_add_path(props, S_HISTORY_KEY_ICON_CONFIG_PATH,
            T_HISTORY_KEY_ICON_CONFIG_PATH, OBS_PATH_FILE,
            filter_text.c_str(),
            key_icon_config_path.c_str());

        obs_properties_add_int(props, S_HISTORY_ICON_H_SPACE,
            T_HISTORY_ICON_H_SPACE, -999, 999, 1);
        obs_properties_add_int(props, S_HISTORY_ICON_V_SPACE,
            T_HISTORY_ICON_V_SPACE, -999, 999, 1);

        /* Text mode properties*/

        obs_properties_add_path(props, S_HISTORY_KEY_NAME_PATH,
            T_HISTORY_KEY_NAME_PATH, OBS_PATH_FILE,
            filter_text.c_str(), key_names_path.c_str());
        obs_properties_add_bool(props, S_HISTORY_USE_FALLBACK_NAME,
            T_HISTORY_USE_FALLBACK_NAMES);

        /* Font */
        obs_properties_add_font(props, S_HISTORY_FONT, T_HISTORY_FONT);
        obs_properties_add_color(props, S_HISTORY_FONT_COLOR,
            T_HISTORY_FONT_COLOR);
        obs_properties_add_int_slider(props, S_HISTORY_OPACITY, T_HISTORY_OPACITY,
            0, 100, 1);
        obs_properties_add_bool(props, S_HISTORY_OUTLINE, T_HISTORY_OUTLINE);

        obs_properties_add_int(props, S_HISTORY_OUTLINE_SIZE,
            T_HISTORY_OUTLINE_SIZE, 1, 20, 1);
        obs_properties_add_color(props, S_HISTORY_OUTLINE_COLOR,
            T_HISTORY_OUTLINE_COLOR);
        obs_properties_add_int_slider(props, S_HISTORY_OUTLINE_OPACITY,
            T_HISTORY_OUTLINE_OPACITY, 0, 100, 1);

        /* Other */
        const auto icon_dir_list = obs_properties_add_list(
            props, S_HISTORY_DIRECTION,
            T_HISTORY_DIRECTION, OBS_COMBO_TYPE_LIST,
            OBS_COMBO_FORMAT_INT);

        const auto include_pad = obs_properties_add_bool(
            props, S_HISTORY_INCLUDE_PAD, T_HISTORY_INCLUDE_PAD);

        obs_property_list_add_int(icon_dir_list, T_HISTORY_DIRECTION_DOWN, 0);
        obs_property_list_add_int(icon_dir_list, T_HISTORY_DIRECTION_UP, 1);
        obs_property_list_add_int(icon_dir_list, T_HISTORY_DIRECTION_LEFT, 2);
        obs_property_list_add_int(icon_dir_list, T_HISTORY_DIRECTION_RIGHT, 3);

        obs_properties_add_int(props, S_HISTORY_SIZE,
            T_HISTORY_HISTORY_SIZE, 1, MAX_HISTORY_SIZE, 1);
        obs_properties_add_bool(props, S_HISTORY_FIX_CUTTING,
            T_HISTORY_FIX_CUTTING);
        obs_properties_add_bool(props, S_HISTORY_INCLUDE_MOUSE,
            T_HISTORY_INCLUDE_MOUSE);

        obs_property_set_modified_callback(include_pad, include_pad_changed);
        obs_properties_add_int(props, S_CONTROLLER_ID, T_CONTROLLER_ID, 0, 3, 1);
        obs_properties_add_int(props, S_HISTORY_INTERVAL, T_HISTORY_INTERVAL, 1,
            1000, 1);

        obs_properties_add_bool(props, S_HISTORY_ENABLE_REPEAT_KEYS,
            T_HISTORY_ENABLE_REPEAT_KEYS);
        obs_properties_add_bool(props, S_HISTORY_ENABLE_AUTO_CLEAR,
            T_HISTORY_ENABLE_AUTO_CLEAR);
        obs_properties_add_int(props, S_HISTORY_AUTO_CLEAR_INTERVAL,
            T_HISTORY_AUTO_CLEAR_INTERVAL, 1, 30, 1);

        /* Command mode */
        obs_properties_add_bool(props, S_HISTORY_COMMAND_MODE,
            T_HISTORY_COMMAND_MODE);

        obs_properties_add_button(props, S_HISTORY_CLEAR_HISTORY,
            T_HISTORY_CLEAR_HISTORY, clear_history);
        return props;
    }

    void register_history()
    {
        obs_source_info si = {};
        si.id = "input-history";
        si.type = OBS_SOURCE_TYPE_INPUT;
        si.output_flags = OBS_SOURCE_VIDEO;
        si.get_properties = get_properties_for_history;

        si.get_name = [](void*) { return obs_module_text("InputHistory"); };
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
