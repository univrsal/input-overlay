/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include <obs-frontend-api.h>
#include <cstring>
#include "input_source.hpp"
#include "../hook/hook_helper.hpp"
#include "../hook/gamepad_hook.hpp"
#include "../util/element/element_data_holder.hpp"
#include "../util/util.hpp"
#include "../../ccl/ccl.hpp"
#include "util/layout_constants.hpp"
#include "util/config-file.h"
#include "network/remote_connection.hpp"
#include "network/io_server.hpp"

namespace sources
{
    inline void input_source::update(obs_data_t* settings)
    {
        m_settings.selected_source = obs_data_get_int(settings, S_INPUT_SOURCE);

        const auto config = obs_data_get_string(settings, S_LAYOUT_FILE);
        m_settings.image_file = obs_data_get_string(settings, S_OVERLAY_FILE);

        if (m_settings.layout_file != config) /* Only reload config file if path changed */
        {
            m_settings.layout_file = config;
            m_overlay->load();
        }
        
        m_settings.gamepad = obs_data_get_int(settings, S_CONTROLLER_ID);
#ifdef _WIN32
        m_settings.left_dz = obs_data_get_int(settings, S_CONTROLLER_L_DEAD_ZONE) / STICK_MAX_VAL;
        m_settings.right_dz = obs_data_get_int(settings, S_CONTROLLER_R_DEAD_ZONE) / STICK_MAX_VAL;
#endif
        m_settings.mouse_sens = obs_data_get_int(settings, S_MOUSE_SENS);

        if ((m_settings.use_center = obs_data_get_bool(settings, S_MONITOR_USE_CENTER)))
        {        
            m_settings.monitor_h = obs_data_get_int(settings, S_MONITOR_H_CENTER);
            m_settings.monitor_w = obs_data_get_int(settings, S_MONITOR_V_CENTER);
            m_settings.mouse_deadzone = obs_data_get_int(settings, S_MOUSE_DEAD_ZONE);
        }
    }

    inline void input_source::tick(float seconds)
    {
        if (m_overlay->is_loaded())
        {
            m_overlay->refresh_data();
        }
    }

    inline void input_source::render(gs_effect_t* effect) const
    {
        if (!m_overlay->get_texture() || !m_overlay->get_texture()->texture)
            return;

        if (m_settings.layout_file.empty() || !m_overlay->is_loaded())
        {
            gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"),
                m_overlay->get_texture()->texture);
            gs_draw_sprite(m_overlay->get_texture()->texture, 0, cx, cy);
        }
        else
        {
            m_overlay->draw(effect);
        }
    }

    void input_source::load_text_source()
    {
        if (config_get_bool(obs_frontend_get_global_config(), S_REGION, S_TEXT))
        {
#ifdef _WIN32
            m_settings.text_source = obs_source_create("text_gdiplus\0", "overlay-text-source",
                m_settings.data, nullptr);
#else
            m_text_source = obs_source_create("text_ft2_source\0", "overlay-text-source", m_source_settings, nullptr);
#endif
            obs_source_add_active_child(m_source, m_settings.text_source);
        }
    }

    void input_source::unload_text_source()
    {
        if (m_settings.text_source)
        {
            obs_source_remove(m_settings.text_source);
            obs_source_release(m_settings.text_source);
            m_settings.text_source = nullptr;
        }
    }

    bool path_changed(obs_properties_t* props, obs_property_t* p,
        obs_data_t* s)
    {
        const std::string cfg = obs_data_get_string(s, S_LAYOUT_FILE);
        auto temp = ccl_config(cfg, "");

        const auto flags = temp.get_int(CFG_FLAGS, true);

        obs_property_set_visible(GET_PROPS(S_CONTROLLER_L_DEAD_ZONE), flags & FLAG_LEFT_STICK);
        obs_property_set_visible(GET_PROPS(S_CONTROLLER_R_DEAD_ZONE), flags & FLAG_RIGHT_STICK);
        obs_property_set_visible(GET_PROPS(S_CONTROLLER_ID), flags & FLAG_GAMEPAD
            || (flags & FLAG_LEFT_STICK || flags & FLAG_RIGHT_STICK));
        obs_property_set_visible(GET_PROPS(S_MOUSE_SENS), flags & FLAG_MOUSE);
        obs_property_set_visible(GET_PROPS(S_MONITOR_USE_CENTER), flags & FLAG_MOUSE);
        obs_property_set_visible(GET_PROPS(S_MOUSE_DEAD_ZONE), flags & FLAG_MOUSE);

        if (config_get_bool(obs_frontend_get_global_config(), S_REGION, S_TEXT))
            obs_property_set_visible(GET_PROPS(S_OVERLAY_FONT), true);// flags & FLAG_TEXT); // TODO: temporary
        return true;
    }

    bool use_monitor_center_changed(obs_properties_t* props, obs_property_t* p, obs_data_t* data)
    {
        const auto use_center = obs_data_get_bool(data, S_MONITOR_USE_CENTER);
        obs_property_set_visible(GET_PROPS(S_MONITOR_H_CENTER), use_center);
        obs_property_set_visible(GET_PROPS(S_MONITOR_V_CENTER), use_center);

        return true;
    }

    bool reload_connections(obs_properties_t* props, obs_property_t* property, void* data)
    {
		auto connection_list = obs_properties_get(props, S_INPUT_SOURCE);
		auto cfg = obs_frontend_get_global_config();      
        if (connection_list)
			network::server_instance->get_clients(connection_list, network::local_input);

		return true;
    }

    bool toggle_font_settings(obs_properties_t* props, obs_property_t* p, obs_data_t* data)
    {
        const auto show = obs_data_get_bool(data, S_OVERLAY_FONT);
        auto prop = obs_properties_first(props);
        auto use_gradient = obs_data_get_bool(data, "gradient");
        auto use_outline = obs_data_get_bool(data, "outline");

        for (; prop; obs_property_next(&prop))
        {
            auto name = obs_property_name(prop);

            if (!strcmp(name, "read_from_file") || !strcmp(name, "text") ||
                !strcmp(name, "file") || !strcmp(name, "vertical") ||
                strstr(name, "align") || strstr(name, "extents") ||
                strstr(name, "chatlog")
                || (!use_gradient && strstr(name, "gradient") && strlen(name) > strlen("gradient"))
                || (!use_outline && strstr(name, "outline") && strlen(name) > strlen("outline")))
            {  /* do not show unnecessary properties */
                obs_property_set_visible(prop, false);
            }
            else if (!strstr(name, "io"))
            {
                obs_property_set_visible(prop, show);
            }
        }
        return true;
    }

    bool reload_pads(obs_properties_t* props, obs_property_t* property,
        void* data)
    {
        UNUSED_PARAMETER(props);
        UNUSED_PARAMETER(property);
        UNUSED_PARAMETER(data);

        gamepad::init_pad_devices();
        return true;
    }

    obs_properties_t* get_properties_for_overlay(void* data)
    {
        std::string img_path;
        std::string layout_path;
        auto config = obs_frontend_get_global_config();
        const auto s = static_cast<input_source*>(data);
        obs_properties_t* props = nullptr;

        /* Font settings */
        if (config_get_bool(config, S_REGION, S_TEXT) && s->m_settings.text_source)
        {
            props = obs_source_properties(s->m_settings.text_source);
            auto prop = obs_properties_first(props);
            
            for (;prop; obs_property_next(&prop)) /* Hide font properties by default */
                obs_property_set_visible(prop, false);

            auto font_settings = obs_properties_add_bool(props, S_OVERLAY_FONT, T_OVERLAY_FONT);
            obs_property_set_modified_callback(font_settings, toggle_font_settings);
        }
        else
        {
            props = obs_properties_create();
        }
        /* If enabled add dropdown to select input source */
        if (config_get_bool(config, S_REGION, S_REMOTE))
        {
			auto list = obs_properties_add_list(props, S_INPUT_SOURCE, T_INPUT_SOURCE, OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);
			obs_properties_add_button(props, S_RELOAD_CONNECTIONS, T_RELOAD_CONNECTIONS, reload_connections);
            if (network::network_flag)
            {
				network::server_instance->get_clients(list, network::local_input);
            }
        }

        auto filter_img = util_file_filter(
            T_FILTER_IMAGE_FILES, "*.jpg *.png *.bmp");
        auto filter_text = util_file_filter(T_FILTER_TEXT_FILES, "*.ini");

        /* Config and texture file path */
        obs_properties_add_path(props, S_OVERLAY_FILE, T_TEXTURE_FILE,
            OBS_PATH_FILE,
            filter_img.c_str(), img_path.c_str());

        const auto cfg = obs_properties_add_path(props, S_LAYOUT_FILE, T_LAYOUT_FILE,
            OBS_PATH_FILE,
            filter_text.c_str(), layout_path.c_str());

        obs_property_set_modified_callback(cfg, path_changed);

        /* Mouse stuff */
        obs_property_set_visible(obs_properties_add_int_slider(props,
            S_MOUSE_SENS, T_MOUSE_SENS, 1, 500, 1), false);

        auto use_center = obs_properties_add_bool(
            props, S_MONITOR_USE_CENTER, T_MONITOR_USE_CENTER);
        obs_property_set_modified_callback(use_center,
            use_monitor_center_changed);

        obs_property_set_visible(obs_properties_add_int(props, S_MONITOR_H_CENTER, T_MONITOR_H_CENTER,
            -9999, 9999, 1), false);
        obs_property_set_visible(obs_properties_add_int(props, S_MONITOR_V_CENTER, T_MONITOR_V_CENTER,
            -9999, 9999, 1), false);
        obs_property_set_visible(obs_properties_add_int_slider(props, S_MOUSE_DEAD_ZONE, T_MOUSE_DEAD_ZONE,
            0, 500, 1), false);

        /* Gamepad stuff */
        obs_property_set_visible(obs_properties_add_int(props, S_CONTROLLER_ID,
            T_CONTROLLER_ID, 0, 3, 1), false);

#if _WIN32 /* Linux only allows analog stick values 0 - 127 -> No reason for a deadzone */
        obs_property_set_visible(obs_properties_add_int_slider(props, S_CONTROLLER_L_DEAD_ZONE,
            T_CONROLLER_L_DEADZONE, 1,
            STICK_MAX_VAL - 1, 1), false);
        obs_property_set_visible(obs_properties_add_int_slider(props, S_CONTROLLER_R_DEAD_ZONE,
            T_CONROLLER_R_DEADZONE, 1,
            STICK_MAX_VAL - 1, 1), false);
#else
        obs_properties_add_button(props, S_RELOAD_PAD_DEVICES, T_RELOAD_PAD_DEVICES, reload_pads);
#endif

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

        si.get_name = [](void*) { return obs_module_text("InputOverlay"); };
        si.create = [](obs_data_t* settings, obs_source_t* source)
        {
            return (void*)new input_source(source, settings);
        };
        si.destroy = [](void* data)
        {
            delete reinterpret_cast<input_source*>(data);
        };
        si.get_width = [](void* data)
        {
            return reinterpret_cast<input_source*>(data)->m_settings.cx;
        };
        si.get_height = [](void* data)
        {
            return reinterpret_cast<input_source*>(data)->m_settings.cy;
        };

        si.get_defaults = [](obs_data_t* settings)
        {
            /* NO-OP */
        };

        si.update = [](void* data, obs_data_t* settings)
        {
            reinterpret_cast<input_source*>(data)->update(settings);
        };
        si.video_tick = [](void* data, float seconds)
        {
            reinterpret_cast<input_source*>(data)->tick(seconds);
        };
        si.video_render = [](void* data, gs_effect_t* effect)
        {
            reinterpret_cast<input_source*>(data)->render(effect);
        };
        obs_register_source(&si);
    }
}
