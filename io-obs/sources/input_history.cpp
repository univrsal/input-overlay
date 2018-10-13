/**
 * This file is part of input-overlay
 * which is licensed under the
 * MOZILLA PUBLIC LICENCE 2.0 - mozilla.org/en-US/MPL/2.0/
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include <iomanip>
#include <sstream>
#include "input_history.hpp"
#include "../util/element/element_data_holder.hpp"
#include "../util/element/element_button.hpp"
#include "../util/element/element_mouse_wheel.hpp"

namespace sources
{
    void input_history_source::load_text_source()
    {
#ifdef WINDOWS
        m_text_source = obs_source_create("text_gdiplus\0", "history-text-source",
            m_settings, nullptr);
#else
		m_text_source = obs_source_create("text_ft2_source\0", "history-text-source", m_settings, NULL);
#endif
        obs_source_add_active_child(m_source, m_text_source);
    }

    void input_history_source::load_icons()
    {
        if (m_key_icons == nullptr)
            m_key_icons = new key_icons();
        m_key_icons->load_from_file(m_key_icon_path, m_key_icon_config_path);
    }

    void input_history_source::load_translation()
    {
        if (m_key_names == nullptr)
            m_key_names = new key_names();
        m_key_names->load_from_file(m_key_name_path);
    }

    void input_history_source::load_command_handler()
    {
        if (!m_command_handler)
            m_command_handler = new command_handler();
    }

    void input_history_source::unload_text_source()
    {
        obs_source_remove(m_text_source);
        obs_source_release(m_text_source);
        m_text_source = nullptr;
    }

    void input_history_source::unload_icons()
    {
        if (m_key_icons)
        {
            delete m_key_icons;
            m_key_icons = nullptr;
        }
    }

    void input_history_source::unload_translation()
    {
        if (m_key_names)
        {
            delete m_key_names;
            m_key_names = nullptr;
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

    void input_history_source::add_to_history(key_bundle b)
    {
        for (int i = MAX_HISTORY_SIZE - 1; i > 0; i--)
        {
            m_history[i] = m_history[i - 1];
        }
        m_history[0] = b;
    }

    void input_history_source::clear_history()
    {
        for (auto& i : m_history)
        {
            i = {};
        }

        if (GET_MASK(MASK_COMMAND_MODE) && m_command_handler)
            m_command_handler->clear();

        m_prev_keys = {};
        m_current_keys = {};

        obs_data_set_string(m_settings, "text", "");
        obs_source_update(m_text_source, m_settings);
        obs_source_update(m_source, m_settings);
    }

    key_bundle input_history_source::check_keys() const
    {
        auto temp = key_bundle();
        if (!hook::input_data->is_empty() || GET_MASK(MASK_INCLUDE_PAD))
        {
            for (const auto& data : hook::input_data->m_data)
            {
                if (!GET_MASK(MASK_INCLUDE_PAD) && (data.first & VC_PAD_MASK))
                    continue;

                if (!GET_MASK(MASK_INCLUDE_MOUSE) && (data.first & VC_MOUSE_MASK))
                    continue;

                const auto btn = data.second->get_type() == BUTTON;
                const auto wheel = !btn && data.second->get_type() ==
                    MOUSE_SCROLLWHEEL;

                if (btn)
                {
                    const auto btn_data = reinterpret_cast<element_data_button*>(
                        data.second.get());

                    if (btn_data && btn_data->get_state() == STATE_PRESSED)
                        temp.add_key(data.first);
                }
                else if (wheel)
                {
                    element_data_wheel* wheel_data =
                        reinterpret_cast<element_data_wheel*>(data.second.get());
                    if (wheel_data)
                    {
                        if (wheel_data->get_dir() == WHEEL_DIR_UP)
                            temp.add_key(VC_MOUSE_WHEEL_UP);
                        else if (wheel_data->get_dir() == WHEEL_DIR_DOWN)
                            temp.add_key(VC_MOUSE_WHEEL_DOWN);
                    }
                }
            }
        }

        return temp;
    }

    void input_history_source::handle_text_history()
    {
        std::string text;
        std::string line;

        if (GET_MASK(MASK_COMMAND_MODE) && m_command_handler)
        {
            text = m_command_handler->
                get_history(m_history_direction == DIR_DOWN);
        }
        else
        {
#define START ((m_history_direction == DIR_UP || m_history_direction == DIR_LEFT) ? m_history_size - 1 : 0)
#define CONDITION ((m_history_direction == DIR_UP || m_history_direction == DIR_LEFT) ? (i >= 0) : (i < m_history_size))
#define INCREMENT (i += ((m_history_direction == DIR_UP || m_history_direction == DIR_LEFT) ? -1 : 1))

            for (int i = START; CONDITION; INCREMENT)
            {
                if (m_history[i].m_empty)
                {
                    text.append("\n");
                    continue;
                }

                line = m_history[i].to_string(m_bool_values, m_key_names);

                if (!line.empty())
                {
                    if (m_history_direction != DIR_UP && i > 0 ||
                        m_history_direction == DIR_UP && i < m_history_size -
                        1)
                        text.append("\n");
                    text.append(line);
                }
            }
        }

        obs_data_set_string(m_settings, "text", text.c_str());
        obs_source_update(m_text_source, m_settings);
        obs_source_update(m_source, m_settings);
    }

    void input_history_source::handle_icon_history(gs_effect_t* effect)
    {
        if (m_key_icons && m_key_icons->is_loaded())
        {
            gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"),
                m_key_icons->get_texture()->texture);

            int index = 0, index2 = 0;
            key_icon* icon = nullptr;
            switch (m_history_direction)
            {
            case DIR_UP:
            case DIR_DOWN:
                for (int i = START; CONDITION; INCREMENT)
                {
                    index2 = 0;
                    for (unsigned short key : m_history[i].m_keys)
                    {
                        icon = m_key_icons->get_icon_for_key(key);
                        if (icon)
                        {
                            gs_matrix_push();

                            gs_matrix_translate3f(
                                (float)(m_key_icons->get_w() + m_icon_h_space) *
                                index2,
                                (float)(m_key_icons->get_h() + m_icon_v_space) *
                                index, 1.f);
                            gs_draw_sprite_subregion(
                                m_key_icons->get_texture()->texture, 0, icon->u,
                                icon->v,
                                m_key_icons->get_w() + 1,
                                m_key_icons->get_h() + 1);
                            gs_matrix_pop();
                            index2++;
                        }
                    }
                    index++;
                }
                break;
            case DIR_LEFT:
            case DIR_RIGHT:
                for (int i = START; CONDITION; INCREMENT)
                {
                    index2 = 0;
                    for (unsigned short key : m_history[i].m_keys)
                    {
                        icon = m_key_icons->get_icon_for_key(key);
                        if (icon)
                        {
                            gs_matrix_push();
                            gs_matrix_translate3f(
                                (float)(m_key_icons->get_w() + m_icon_h_space) *
                                index,
                                (float)(m_key_icons->get_h() + m_icon_v_space) *
                                index2, 1.f);

                            gs_draw_sprite_subregion(
                                m_key_icons->get_texture()->texture, 0, icon->u,
                                icon->v,
                                m_key_icons->get_w() + 1,
                                m_key_icons->get_h() + 1);
                            gs_matrix_pop();
                            index2++;
                        }
                    }
                    index++;
                }
            }
        }
    }

    inline void input_history_source::update(obs_data_t* settings)
    {
        obs_source_update(m_text_source, settings);

        SET_MASK(MASK_TEXT_MODE, obs_data_get_int(settings, S_OVERLAY_MODE) == 0);
        SET_MASK(MASK_INCLUDE_MOUSE, obs_data_get_bool(settings,
            S_OVERLAY_INCLUDE_MOUSE));
        SET_MASK(MASK_REPEAT_KEYS, obs_data_get_bool(settings,
            S_OVERLAY_ENABLE_REPEAT_KEYS));
        SET_MASK(MASK_AUTO_CLEAR, obs_data_get_bool(settings,
            S_OVERLAY_ENABLE_AUTO_CLEAR));
        SET_MASK(MASK_FIX_CUTTING, obs_data_get_bool(settings,
            S_OVERLAY_FIX_CUTTING));
        SET_MASK(MASK_USE_FALLBACK, obs_data_get_bool(settings,
            S_OVERLAY_USE_FALLBACK_NAME));
        SET_MASK(MASK_COMMAND_MODE, obs_data_get_bool(settings,
            S_OVERLAY_COMMAND_MODE));
        SET_MASK(MASK_INCLUDE_PAD, obs_data_get_bool(settings,
            S_OVERLAY_INCLUDE_PAD));

        m_update_interval = obs_data_get_int(settings, S_OVERLAY_INTERVAL);
        m_clear_interval = obs_data_get_int(settings,
            S_OVERLAY_AUTO_CLEAR_INTERVAL);

        m_key_name_path = obs_data_get_string(settings, S_OVERLAY_KEY_NAME_PATH);
        m_key_icon_config_path = obs_data_get_string(
            settings, S_OVERLAY_KEY_ICON_CONFIG_PATH);
        m_key_icon_path = obs_data_get_string(settings, S_OVERLAY_KEY_ICON_PATH);

        m_icon_h_space = obs_data_get_int(settings, S_OVERLAY_ICON_H_SPACE);
        m_icon_v_space = obs_data_get_int(settings, S_OVERLAY_ICON_V_SPACE);

        m_history_size = obs_data_get_int(settings, S_OVERLAY_HISTORY_SIZE);
        m_history_direction = static_cast<icon_direction>(obs_data_get_int(
            settings, S_OVERLAY_DIRECTION));

        if (GET_MASK(MASK_COMMAND_MODE))
        {
            load_command_handler();
        }
        else
        {
            unload_command_handler();
        }

        if (GET_MASK(MASK_TEXT_MODE))
        {
            unload_icons();
        }
        else
        {
            load_icons();
        }

        if (m_key_name_path.empty())
        {
            SET_MASK(MASK_TRANSLATION, false);
            unload_translation();
        }
        else
        {
            SET_MASK(MASK_TRANSLATION, true);
            load_translation();
        }

        if (GET_MASK(MASK_INCLUDE_PAD))
        {
            m_pad_id = static_cast<uint8_t>(obs_data_get_int(
                settings, S_CONTROLLER_ID));
        }
    }

    inline void input_history_source::tick(float seconds)
    {
        if (!m_source || !obs_source_showing(m_source))
            return;

        if (GET_MASK(MASK_AUTO_CLEAR))
        {
            m_clear_timer += seconds;
            if (m_clear_timer >= m_clear_interval)
            {
                m_clear_timer = 0.f;
                clear_history();
            }
        }

        if (GET_MASK(MASK_INCLUDE_PAD))
        {
            /* TODO: incomplete? */
        }

        if (GET_MASK(MASK_COMMAND_MODE) && m_command_handler)
        {
            if (hook::last_character != 0)
            {
                m_command_handler->handle_char(hook::last_character);
                hook::last_character = 0;
                handle_text_history();
            }
        }
        else
        {
            if (m_counter >= m_update_interval)
            {
                m_counter = 0;
                if (!m_current_keys.m_empty)
                {
                    if (GET_MASK(MASK_TEXT_MODE) || m_key_icons && m_key_icons->
                        has_texture_for_bundle(&m_current_keys))
                    {
                        if (GET_MASK(MASK_REPEAT_KEYS) || !m_current_keys.compare(
                            &m_prev_keys))
                        {
                            if (!m_current_keys.is_only_mouse() || GET_MASK(
                                MASK_INCLUDE_MOUSE))
                            {
                                add_to_history(m_current_keys);
                                m_clear_timer = 0.f;
                            }

                            m_prev_keys = m_current_keys;

                            if (GET_MASK(MASK_TEXT_MODE))
                            {
                                handle_text_history();
                            }

                            m_current_keys = {};
                        }
                    }
                }
            }
            else
            {
                m_current_keys.merge(check_keys());
                m_counter++;
            }
        }

        if (GET_MASK(MASK_TEXT_MODE))
        {
            cx = UTIL_MAX(obs_source_get_width(m_text_source), 50);
            cy = UTIL_MAX(obs_source_get_height(m_text_source), 50);
        }
        else
        {
            if (m_key_icons)
            {
                if (m_history_direction == DIR_UP || m_history_direction ==
                    DIR_DOWN)
                {
                    cx = (m_key_icons->get_w() + m_icon_h_space) *
                        MAX_HISTORY_SIZE;
                    cy = (m_key_icons->get_h() + m_icon_v_space) * m_history_size;
                }
                else
                {
                    cx = (m_key_icons->get_w() + m_icon_h_space) * m_history_size;
                    cy = (m_key_icons->get_h() + m_icon_v_space) *
                        MAX_HISTORY_SIZE;
                }
            }
            else
            {
                cx = 50;
                cy = 50;
            }
        }
    }

    inline void input_history_source::render(gs_effect_t* effect)
    {
        if (GET_MASK(MASK_TEXT_MODE))
        {
            obs_source_video_render(m_text_source);
        }
        else
        {
            handle_icon_history(effect);
        }
    }

    void key_bundle::merge(key_bundle other)
    {
        if (!other.m_empty)
        {
            m_empty = false;

            for (int i = 0; i < MAX_SIMULTANEOUS_KEYS; i++)
            {
                if (other.m_keys[i] > 0)
                    m_keys[i] = other.m_keys[i];
            }
        }
    }

    std::string key_bundle::to_string(uint8_t masks, key_names* names)
    {
        if (m_empty)
            return "";

        std::string text;
        bool flag = false;

        for (unsigned short key : m_keys)
        {
            if (key == 0)
                break; /* Array is filled from beginning to end
				          -> First entry with zero means there are none after it */

            if (!(masks & MASK_INCLUDE_MOUSE))
            {
                switch (key)
                {
                case VC_MOUSE_BUTTON1:
                case VC_MOUSE_BUTTON2:
                case VC_MOUSE_BUTTON3:
                case VC_MOUSE_BUTTON4:
                case VC_MOUSE_BUTTON5:
                case VC_MOUSE_WHEEL_UP:
                case VC_MOUSE_WHEEL_DOWN:
                    continue;
                }
            }

            if (key > 0)
            {
                const char* temp = nullptr;

                if (masks & MASK_TRANSLATION)
                {
                    temp = names->get_name(key);

                    if (!temp && (masks & MASK_USE_FALLBACK))
                        temp = key_to_text(key);
                }
                else
                {
                    temp = key_to_text(key);
                }

                if (temp)
                {
                    if (flag)
                        text.append(" + ");
                    else
                        flag = true;
                    text.append(temp);
                }
#ifdef DEBUG
                else
                {
                    if (flag)
                        text.append(" + ");
                    else
                        flag = true;
                    std::stringstream stream;
                    stream << "0x" << std::setfill('0') << std::setw(
                            sizeof(uint16_t) * 2)
                        << std::hex << key;
                    text.append(stream.str());
                }
#endif
            }
        }

        if ((masks & MASK_FIX_CUTTING) && !text.empty())
            text.append(" ");
        return text;
    }

    bool key_bundle::compare(key_bundle* other)
    {
        if (m_empty != other->m_empty)
            return false;

        for (auto i = 0; i < MAX_SIMULTANEOUS_KEYS; i++)
        {
            if (m_keys[i] != other->m_keys[i])
                return false;
        }
        return true;
    }

    bool key_bundle::is_only_mouse()
    {
        for (auto key : m_keys)
        {
            switch (key)
            {
            case 0:
            case VC_MOUSE_BUTTON1:
            case VC_MOUSE_BUTTON2:
            case VC_MOUSE_BUTTON3:
            case VC_MOUSE_BUTTON4:
            case VC_MOUSE_BUTTON5:
            case VC_MOUSE_WHEEL_UP:
            case VC_MOUSE_WHEEL_DOWN:
                break;
            default:
                return false;
            }
        }
        return true;
    }

    void key_bundle::add_key(const uint16_t key)
    {
        if (m_index > MAX_SIMULTANEOUS_KEYS)
        {
            blog(LOG_WARNING,
                "[input-overlay] Input history source collected more than %i keys!\n",
                MAX_SIMULTANEOUS_KEYS);
            return;
        }

        m_keys[m_index] = key;
        m_index++;
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
        const auto state_text = obs_data_get_int(s, S_OVERLAY_MODE) == 0;

        TEXT_VIS(GET_PROPS(S_OVERLAY_FONT));
        TEXT_VIS(GET_PROPS(S_OVERLAY_FONT_COLOR));
        TEXT_VIS(GET_PROPS(S_OVERLAY_OUTLINE));
        TEXT_VIS(GET_PROPS(S_OVERLAY_OUTLINE_SIZE));
        TEXT_VIS(GET_PROPS(S_OVERLAY_OUTLINE_COLOR));
        TEXT_VIS(GET_PROPS(S_OVERLAY_OUTLINE_OPACITY));
        TEXT_VIS(GET_PROPS(S_OVERLAY_FIX_CUTTING));
        TEXT_VIS(GET_PROPS(S_OVERLAY_OPACITY));
        TEXT_VIS(GET_PROPS(S_OVERLAY_KEY_NAME_PATH));
        TEXT_VIS(GET_PROPS(S_OVERLAY_USE_FALLBACK_NAME));
        TEXT_VIS(GET_PROPS(S_OVERLAY_COMMAND_MODE));

        ICON_VIS(GET_PROPS(S_OVERLAY_KEY_ICON_CONFIG_PATH));
        ICON_VIS(GET_PROPS(S_OVERLAY_KEY_ICON_PATH));
        ICON_VIS(GET_PROPS(S_OVERLAY_ICON_V_SPACE));
        ICON_VIS(GET_PROPS(S_OVERLAY_ICON_H_SPACE));

        return true;
    }

    bool include_pad_changed(obs_properties* props, obs_property_t* p,
        obs_data_t* s)
    {
        const auto id = obs_properties_get(props, S_CONTROLLER_ID);
        obs_property_set_visible(id, obs_data_get_bool(s, S_OVERLAY_INCLUDE_PAD));
        return true;
    }

    obs_properties_t* get_properties_for_history(void* data)
    {
        const auto s = reinterpret_cast<input_history_source*>(data);
        const auto props = obs_properties_create();
        const auto mode_list = obs_properties_add_list(props, S_OVERLAY_MODE,
            T_OVERLAY_MODE,
            OBS_COMBO_TYPE_LIST,
            OBS_COMBO_FORMAT_INT);
        obs_property_list_add_int(mode_list, T_OVERLAY_MODE_TEXT, 0);
        obs_property_list_add_int(mode_list, T_OVERLAY_MODE_ICON, 1);
        obs_property_set_modified_callback(mode_list, mode_changed);

        /* Key name file */
        auto filter_img = util_file_filter(
            T_FILTER_IMAGE_FILES, "*.jpg *.png *.bmp");
        auto filter_text = util_file_filter(T_FILTER_TEXT_FILES, "*.ini");
        std::string key_names_path, key_icon_path, key_icon_config_path;

        if (s)
        {
            if (!s->m_key_name_path.empty())
            {
                key_names_path = s->m_key_name_path;
                util_format_path(key_names_path);
            }

            if (!s->m_key_icon_path.empty())
            {
                key_icon_path = s->m_key_icon_path;
                util_format_path(key_icon_path);
            }

            if (!s->m_key_icon_config_path.empty())
            {
                key_icon_config_path = s->m_key_icon_config_path;
                util_format_path(key_icon_config_path);
            }
        }

        /* Icon mode propterties */
        obs_properties_add_path(props, S_OVERLAY_KEY_ICON_PATH,
            T_OVERLAY_KEY_ICON_PATH, OBS_PATH_FILE,
            filter_img.c_str(), key_icon_path.c_str());

        obs_properties_add_path(props, S_OVERLAY_KEY_ICON_CONFIG_PATH,
            T_OVERLAY_KEY_ICON_CONFIG_PATH, OBS_PATH_FILE,
            filter_text.c_str(),
            key_icon_config_path.c_str());

        obs_properties_add_int(props, S_OVERLAY_ICON_H_SPACE,
            T_OVERLAY_ICON_H_SPACE, -999, 999, 1);
        obs_properties_add_int(props, S_OVERLAY_ICON_V_SPACE,
            T_OVERLAY_ICON_V_SPACE, -999, 999, 1);

        /* Text mode properties*/

        obs_properties_add_path(props, S_OVERLAY_KEY_NAME_PATH,
            T_OVERLAY_KEY_NAME_PATH, OBS_PATH_FILE,
            filter_text.c_str(), key_names_path.c_str());
        obs_properties_add_bool(props, S_OVERLAY_USE_FALLBACK_NAME,
            T_OVERLAY_USE_FALLBACK_NAMES);

        /* Font */
        obs_properties_add_font(props, S_OVERLAY_FONT, T_OVERLAY_FONT);
        obs_properties_add_color(props, S_OVERLAY_FONT_COLOR,
            T_OVERLAY_FONT_COLOR);
        obs_properties_add_int_slider(props, S_OVERLAY_OPACITY, T_OVERLAY_OPACITY,
            0, 100, 1);
        obs_properties_add_bool(props, S_OVERLAY_OUTLINE, T_OVERLAY_OUTLINE);

        obs_properties_add_int(props, S_OVERLAY_OUTLINE_SIZE,
            T_OVERLAY_OUTLINE_SIZE, 1, 20, 1);
        obs_properties_add_color(props, S_OVERLAY_OUTLINE_COLOR,
            T_OVERLAY_OUTLINE_COLOR);
        obs_properties_add_int_slider(props, S_OVERLAY_OUTLINE_OPACITY,
            T_OVERLAY_OUTLINE_OPACITY, 0, 100, 1);

        /* Other */
        const auto icon_dir_list = obs_properties_add_list(
            props, S_OVERLAY_DIRECTION,
            T_OVERLAY_DIRECTION, OBS_COMBO_TYPE_LIST,
            OBS_COMBO_FORMAT_INT);

        const auto include_pad = obs_properties_add_bool(
            props, S_OVERLAY_INCLUDE_PAD, T_OVERLAY_INCLUDE_PAD);

        obs_property_list_add_int(icon_dir_list, T_OVERLAY_DIRECTION_DOWN, 0);
        obs_property_list_add_int(icon_dir_list, T_OVERLAY_DIRECTION_UP, 1);
        obs_property_list_add_int(icon_dir_list, T_OVERLAY_DIRECTION_LEFT, 2);
        obs_property_list_add_int(icon_dir_list, T_OVERLAY_DIRECTION_RIGHT, 3);

        obs_properties_add_int(props, S_OVERLAY_HISTORY_SIZE,
            T_OVERLAY_HISTORY_SIZE, 1, MAX_HISTORY_SIZE, 1);
        obs_properties_add_bool(props, S_OVERLAY_FIX_CUTTING,
            T_OVERLAY_FIX_CUTTING);
        obs_properties_add_bool(props, S_OVERLAY_INCLUDE_MOUSE,
            T_OVERLAY_INCLUDE_MOUSE);

        obs_property_set_modified_callback(include_pad, include_pad_changed);
        obs_properties_add_int(props, S_CONTROLLER_ID, T_CONTROLLER_ID, 0, 3, 1);
        obs_properties_add_int(props, S_OVERLAY_INTERVAL, T_OVERLAY_INTERVAL, 1,
            1000, 1);

        obs_properties_add_bool(props, S_OVERLAY_ENABLE_REPEAT_KEYS,
            T_OVERLAY_ENABLE_REPEAT_KEYS);
        obs_properties_add_bool(props, S_OVERLAY_ENABLE_AUTO_CLEAR,
            T_OVERLAY_ENABLE_AUTO_CLEAR);
        obs_properties_add_int(props, S_OVERLAY_AUTO_CLEAR_INTERVAL,
            T_OVERLAY_AUTO_CLEAR_INTERVAL, 1, 30, 1);

        /* Command mode */
        obs_properties_add_bool(props, S_OVERLAY_COMMAND_MODE,
            T_OVERLAY_COMMAND_MODE);
        obs_properties_add_button(props, S_OVERLAY_CLEAR_HISTORY,
            T_OVERLAY_CLEAR_HISTORY, clear_history);
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
            return (void*)new input_history_source(source, settings);
        };
        si.destroy = [](void* data)
        {
            delete reinterpret_cast<input_history_source*>(data);
        };
        si.get_width = [](void* data)
        {
            return reinterpret_cast<input_history_source*>(data)->cx;
        };
        si.get_height = [](void* data)
        {
            return reinterpret_cast<input_history_source*>(data)->cy;
        };

        si.get_defaults = [](obs_data_t* settings)
        {
            obs_data_set_default_int(settings, S_OVERLAY_HISTORY_SIZE, 1);
            obs_data_set_default_int(settings, S_OVERLAY_INTERVAL, 2);
            obs_data_set_default_int(settings, S_OVERLAY_AUTO_CLEAR_INTERVAL, 2);
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

    void key_names::load_from_file(const std::string& path)
    {
        auto cfg = new ccl_config(path, "");

        if (!cfg->is_empty())
        {
            auto node = cfg->get_first();

            if (!node)
            {
                delete cfg;
                return;
            }

            do
            {
                if (node->get_type() == 2)
                {
                    auto val = node->get_id();
                    uint16_t key_code = std::stoul(val, nullptr, 16);
                    m_names[key_code] = node->get_value();
                }
            }
            while ((node = node->get_next()) != nullptr);
        }

        if (cfg->has_errors())
        {
            blog(LOG_WARNING, "[ccl] %s", cfg->get_error_message().c_str());
        }

        if (cfg)
        {
            delete cfg;
            cfg = nullptr;
        }
    }

    const char* key_names::get_name(const uint16_t vc)
    {
        if (m_names.find(vc) != m_names.end())
        {
            return m_names[vc].c_str();
        }

        return nullptr;
    }

    key_names::~key_names()
    {
        m_names.clear();
    }

    key_icons::~key_icons()
    {
        unload_texture();
        m_icons.clear();
    }

    void key_icons::load_from_file(const std::string& img_path,
        const std::string& cfg_path)
    {
        m_loaded = false;
        if (img_path.empty() || cfg_path.empty())
            return;
        m_icon_texture = new gs_image_file_t();

        //gs_image_file_init(m_icon_texture, img_path.c_str());

        obs_enter_graphics();
        //gs_image_file_init_texture(m_icon_texture);
        obs_leave_graphics();

        auto cfg_loaded = false;

        auto cfg = new ccl_config(cfg_path, "");

        if (!cfg->is_empty())
        {
            cfg_loaded = true;
            m_icon_count = cfg->get_int("icon_count");
            m_icon_w = cfg->get_int("icon_w");
            m_icon_h = cfg->get_int("icon_h");
            const auto node = cfg->get_node("icon_order");

            if (node)
            {
                auto icon_order = node->get_value();
                for (auto i = 0; i < m_icon_count; i++)
                {
                    key_icon ico{};
                    /* TODO: Alternative? */
                    const auto vc = 0x000F; //util_read_hex(icon_order);
                    ico.u = (m_icon_w + 3) * i + 1;
                    ico.v = 1;

                    m_icons[vc] = ico;

                    if (icon_order.empty())
                    {
                        m_icon_count = i;
                        break;
                    }
                }
            }
            else
            {
                blog(LOG_ERROR, "Loading key icons from %s failed. No ccl data!",
                    cfg_path.c_str());
            }
        }

        m_loaded = cfg_loaded && m_icon_texture->loaded;

        if (!m_icon_texture->loaded)
        {
            blog(LOG_ERROR, "Error: Failed to load key icons from %s",
                img_path.c_str());
            unload_texture();
        }

        if (!cfg_loaded)
        {
            blog(LOG_ERROR, "Error: Failed to load key icon config from %s",
                cfg_path.c_str());
        }

        if (cfg->has_errors())
        {
            blog(LOG_WARNING, "[ccl] %s", cfg->get_error_message().c_str());
        }

        if (cfg)
        {
            delete cfg;
            cfg = nullptr;
        }
    }

    key_icon* key_icons::get_icon_for_key(const uint16_t vc)
    {
        if (!m_loaded)
            return nullptr;

        if (m_icons.find(vc) != m_icons.end())
        {
            return &m_icons[vc];
        }

        return nullptr;
    }

    bool key_icons::has_texture_for_bundle(key_bundle* bundle)
    {
        for (unsigned short key : bundle->m_keys)
        {
            /* Any key inside a bundle has icon --> draw the entire bundle */
            if (get_icon_for_key(key)) return true;
        }
        return false;
    }

    void key_icons::unload_texture()
    {
        if (m_icon_texture)
        {
            obs_enter_graphics();
            ////gs_image_file_free(m_icon_texture);
            m_icon_texture = nullptr;
            obs_leave_graphics();
        }
    }
}
