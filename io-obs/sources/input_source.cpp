/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "input_source.hpp"
#include "../hook/hook_helper.hpp"
#include "../hook/gamepad_hook.hpp"
#include "../util/element/element_data_holder.hpp"
#include "../util/util.hpp"
#include "../../ccl/ccl.hpp"
#include <clocale>
#include "../util/element/element_button.hpp"
#include "../util/element/element_mouse_wheel.hpp"

namespace sources
{
    inline void input_source::update(obs_data_t* settings)
    {
        m_is_controller = obs_data_get_bool(settings, S_IS_CONTROLLER);

        if (m_is_controller)
        {
            m_pad_id = static_cast<uint8_t>(obs_data_get_int(settings, S_CONTROLLER_ID));
            m_l_dz = static_cast<uint16_t>(obs_data_get_int(settings, S_CONTROLLER_L_DEAD_ZONE));
            m_r_dz = static_cast<uint16_t>(obs_data_get_int(settings, S_CONTROLLER_R_DEAD_ZONE));
        }

        //m_layout.m_max_mouse_movement = (uint16_t)obs_data_get_int(settings, S_MOUSE_SENS);

        m_image_file = obs_data_get_string(settings, S_OVERLAY_FILE);
        m_layout_file = obs_data_get_string(settings, S_LAYOUT_FILE);

        m_monitor_use_center = obs_data_get_bool(settings, S_MONITOR_USE_CENTER);
        m_mouse_dead_zone = static_cast<uint8_t>(obs_data_get_int(settings, S_MOUSE_DEAD_ZONE));

        if (m_monitor_use_center)
        {
            m_monitor_h = static_cast<uint32_t>(obs_data_get_int(settings, S_MONITOR_H_CENTER));
            m_monitor_v = static_cast<uint32_t>(obs_data_get_int(settings, S_MONITOR_V_CENTER));
        }

        m_overlay->load(m_layout_file, m_image_file);

        if ((m_layout_file.empty() || !m_overlay->is_loaded())
            && m_overlay->get_texture() && m_overlay->get_texture()->texture)
        {
            cx = m_overlay->get_texture()->cx;
            cy = m_overlay->get_texture()->cy;
        }
        else
        {
            cx = m_overlay->get_width();
            cy = m_overlay->get_height();
        }
    }

    inline void input_source::tick(float seconds)
    {
        /* NO-OP */
    }

    inline void input_source::render(gs_effect_t* effect) const
    {
        if (!m_overlay->get_texture() || !m_overlay->get_texture()->texture)
            return;

        if (m_layout_file.empty() || !m_overlay->is_loaded())
        {
            gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), m_overlay->get_texture()->texture);
            gs_draw_sprite(m_overlay->get_texture()->texture, 0, cx, cy);
        }
        else
        {
            m_overlay->draw(effect);
        }
    }

    void input_source::load_overlay() const
    {
        if (m_layout_file.empty() || m_image_file.empty())
            return;

        m_overlay->unload();
        m_overlay->load(m_layout_file, m_image_file);

    }

    bool is_controller_changed(obs_properties_t* props, obs_property_t* p, obs_data_t* s)
    {
        const auto is_gamepad = obs_data_get_bool(s, S_IS_CONTROLLER);

        const auto id = obs_properties_get(props, S_CONTROLLER_ID);
        const auto l_deadzone = obs_properties_get(props, S_CONTROLLER_L_DEAD_ZONE);
        const auto r_deadzone = obs_properties_get(props, S_CONTROLLER_R_DEAD_ZONE);

        obs_property_set_visible(id, is_gamepad);
        obs_property_set_visible(l_deadzone, is_gamepad);
        obs_property_set_visible(r_deadzone, is_gamepad);

        return true;
    }

    bool is_mouse_changed(obs_properties_t* props, obs_property_t* p, obs_data_t* s)
    {
        const auto is_mouse = obs_data_get_bool(s, S_IS_MOUSE);

        const auto sens = obs_properties_get(props, S_MOUSE_SENS);
        const auto use_center = obs_properties_get(props, S_MONITOR_USE_CENTER);
        const auto mon_w = obs_properties_get(props, S_MONITOR_H_CENTER);
        const auto mon_h = obs_properties_get(props, S_MONITOR_V_CENTER);
        const auto dead_zone = obs_properties_get(props, S_MOUSE_DEAD_ZONE);

        obs_property_set_visible(sens, is_mouse);
        obs_property_set_visible(use_center, is_mouse);
        obs_property_set_visible(mon_w, is_mouse);
        obs_property_set_visible(mon_h, is_mouse);
        obs_property_set_visible(dead_zone, is_mouse);

        return true;
    }

    bool use_monitor_center_changed(obs_properties_t* props, obs_property_t* p, obs_data_t* s)
    {
        const auto use_center = obs_data_get_bool(s, S_MONITOR_USE_CENTER);
        obs_property_set_visible(GET_PROPS(S_MONITOR_H_CENTER), use_center);
        obs_property_set_visible(GET_PROPS(S_MONITOR_V_CENTER), use_center);

        return true;
    }

    obs_properties_t* get_properties_for_overlay(void* data)
    {
        const auto s = reinterpret_cast<input_source*>(data);

        const auto props = obs_properties_create();

        std::string img_path;
        std::string layout_path;
        auto filter_img = util_file_filter(T_FILTER_IMAGE_FILES, "*.jpg *.png *.bmp");
        auto filter_text = util_file_filter(T_FILTER_TEXT_FILES, "*.ini");

        if (s)
        {
            if (!s->m_image_file.empty())
            {
                img_path = s->m_image_file;
                util_format_path(img_path);
            }
            if (!s->m_layout_file.empty())
            {
                layout_path = s->m_layout_file;
                util_format_path(layout_path);
            }
        }

        obs_properties_add_path(props, S_OVERLAY_FILE, T_OVERLAY_FILE, OBS_PATH_FILE,
                                filter_img.c_str(), img_path.c_str());

        obs_properties_add_path(props, S_LAYOUT_FILE, T_LAYOUT_FILE, OBS_PATH_FILE,
                                filter_text.c_str(), layout_path.c_str());

        /* Mouse stuff */
        auto is_Mouse = obs_properties_add_bool(props, S_IS_MOUSE, T_IS_MOUSE);
        obs_property_set_modified_callback(is_Mouse, is_mouse_changed);

        obs_properties_add_int_slider(props, S_MOUSE_SENS, T_MOUSE_SENS, 1, 500, 1);

        auto use_center = obs_properties_add_bool(props, S_MONITOR_USE_CENTER, T_MONITOR_USE_CENTER);
        obs_property_set_modified_callback(use_center, use_monitor_center_changed);

        obs_properties_add_int(props, S_MONITOR_H_CENTER, T_MONITOR_H_CENTER, -9999, 9999, 1);
        obs_properties_add_int(props, S_MONITOR_V_CENTER, T_MONITOR_V_CENTER, -9999, 9999, 1);
        obs_properties_add_int_slider(props, S_MOUSE_DEAD_ZONE, T_MOUSE_DEAD_ZONE, 0, 50, 1);

        /* Gamepad stuff */

        auto is_controller = obs_properties_add_bool(props, S_IS_CONTROLLER, T_IS_CONTROLLER);
        obs_property_set_modified_callback(is_controller, is_controller_changed);
        obs_properties_add_int(props, S_CONTROLLER_ID, T_CONTROLLER_ID, 0, 3, 1);

#if HAVE_XINPUT /* Linux only allows values 0 - 127 */
        obs_properties_add_int_slider(props, S_CONTROLLER_L_DEAD_ZONE,
                                      T_CONROLLER_L_DEADZONE, 1, STICK_MAX_VAL - 1, 1);
        obs_properties_add_int_slider(props, S_CONTROLLER_R_DEAD_ZONE,
                                      T_CONROLLER_R_DEADZONE, 1, STICK_MAX_VAL - 1, 1);
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
        si.destroy = [](void* data) { delete reinterpret_cast<input_source*>(data); };
        si.get_width = [](void* data) { return reinterpret_cast<input_source*>(data)->cx; };
        si.get_height = [](void* data) { return reinterpret_cast<input_source*>(data)->cy; };

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
