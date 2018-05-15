#include "input-source.hpp"

/**
 * This file is part of input-overlay
 * which is licenced under the MIT licence.
 * See LICENCE or https://mit-license.org
 * github.com/univrsal/input-overlay
 */

namespace Sources {
/*
void InputSource::draw_key(gs_effect_t *effect, InputKey* key, uint16_t x, uint16_t y, bool rot, float angle)
{
	gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), m_image->texture);

	gs_matrix_push();

	if (rot)
	{
		gs_matrix_translate3f(x + (key->w / 2.f), y + (key->h / 2.f), 1.f); // Put it in position
		gs_matrix_translate3f((float)-(key->w / 2), (float)-(key->h / 2), 1.f); // Needed for rotation
		gs_matrix_rotaa4f(0.f, 0.f, 1.f, angle);
		gs_matrix_translate3f((float)-(key->w / 2), (float)-(key->h / 2), 1.f); // Needed for rotation
		gs_draw_sprite_subregion(m_image->texture, 0, key->texture_u, key->texture_v, key->w + 1, key->h + 1);
	}
	else
	{
		gs_matrix_translate3f((float)x, (float)y, 1.f);

		if (key->m_pressed)
		{
			gs_draw_sprite_subregion(m_image->texture, 0, key->texture_u, key->texture_v + key->h + 3, key->w + 1, key->h + 1);
		}
		else
		{
			gs_draw_sprite_subregion(m_image->texture, 0, key->texture_u, key->texture_v, key->w + 1, key->h + 1);
		}
	}

	gs_matrix_pop();
}

void InputSource::draw_key(gs_effect_t * effect, InputKey * key, uint16_t x, uint16_t y)
{
	draw_key(effect, key, x, y, false, 0.f);
}

void InputSource::draw_key(gs_effect_t * effect, InputKey * key)
{
	draw_key(effect, key, key->column, key->row, false, 0.f);
}
*/
inline void InputSource::Update(obs_data_t *settings)
{
	m_is_controller = obs_data_get_bool(settings, S_IS_CONTROLLER);

	if (m_is_controller)
	{
		m_pad_id = (uint8_t)obs_data_get_int(settings, S_CONTROLLER_ID);
		m_l_dz = (uint16_t)obs_data_get_int(settings, S_CONTROLLER_L_DEAD_ZONE);
		m_r_dz = (uint16_t)obs_data_get_int(settings, S_CONTROLLER_R_DEAD_ZONE);
	}

	//m_layout.m_max_mouse_movement = (uint16_t)obs_data_get_int(settings, S_MOUSE_SENS);

	m_image_file = obs_data_get_string(settings, S_OVERLAY_FILE);
	m_layout_file = obs_data_get_string(settings, S_LAYOUT_FILE);

	m_monitor_use_center = obs_data_get_bool(settings, S_MONITOR_USE_CENTER);
	m_mouse_dead_zone = (uint8_t)obs_data_get_int(settings, S_MOUSE_DEAD_ZONE);

	if (m_monitor_use_center)
	{
		m_monitor_h = (uint32_t)obs_data_get_int(settings, S_MONITOR_H_CENTER);
		m_monitor_v = (uint32_t)obs_data_get_int(settings, S_MONITOR_V_CENTER);
	}

	m_overlay->load(m_layout_file, m_image_file);

	if (!m_overlay->is_loaded() && m_overlay->get_texture() && m_overlay->get_texture()->texture)
	{
		cx = m_image->cx;
		cy = m_image->cy;
	}
}

inline void InputSource::Tick(float seconds)
{
#ifdef HAVE_XINPUT
	update_gamepads(); /* Part of gamepad-hook */
#endif
}

inline void InputSource::Render(gs_effect_t *effect)
{
	if (!m_image || !m_image->texture)
		return;

	if (m_layout_file.empty() || !m_overlay->is_loaded())
	{
		gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), m_image->texture);
		gs_draw_sprite(m_image->texture, 0, cx, cy);
	}
	else
	{
		m_overlay->draw(effect);
	}
}

void InputSource::load_overlay()
{
	if (m_layout_file.empty() || m_image_file.empty())
		return;

	m_overlay.get()->unload();
	m_overlay.get()->load(m_layout_file, m_image_file);
}

bool is_controller_changed(obs_properties_t * props, obs_property_t * p, obs_data_t * s)
{
	bool is_gamepad = obs_data_get_bool(s, S_IS_CONTROLLER);

	obs_property_t *id = obs_properties_get(props, S_CONTROLLER_ID);
	obs_property_t *l_deadzone = obs_properties_get(props, S_CONTROLLER_L_DEAD_ZONE);
	obs_property_t *r_deadzone = obs_properties_get(props, S_CONTROLLER_R_DEAD_ZONE);

	obs_property_set_visible(id, is_gamepad);
	obs_property_set_visible(l_deadzone, is_gamepad);
	obs_property_set_visible(r_deadzone, is_gamepad);

	return true;
}

bool is_mouse_changed(obs_properties_t * props, obs_property_t * p, obs_data_t * s)
{
	bool is_mouse = obs_data_get_bool(s, S_IS_MOUSE);

	obs_property_t * sens = obs_properties_get(props, S_MOUSE_SENS);
	obs_property_t * use_center = obs_properties_get(props, S_MONITOR_USE_CENTER);
	obs_property_t * mon_w = obs_properties_get(props, S_MONITOR_H_CENTER);
	obs_property_t * mon_h = obs_properties_get(props, S_MONITOR_V_CENTER);
	obs_property_t * dead_zone = obs_properties_get(props, S_MOUSE_DEAD_ZONE);

	obs_property_set_visible(sens, is_mouse);
	obs_property_set_visible(use_center, is_mouse);
	obs_property_set_visible(mon_w, is_mouse);
	obs_property_set_visible(mon_h, is_mouse);
	obs_property_set_visible(dead_zone, is_mouse);

	return true;
}

bool use_monitor_center_changed(obs_properties_t * props, obs_property_t * p, obs_data_t * s)
{
	bool use_center = obs_data_get_bool(s, S_MONITOR_USE_CENTER);
	obs_property_set_visible(GET_PROPS(S_MONITOR_H_CENTER), use_center);
	obs_property_set_visible(GET_PROPS(S_MONITOR_V_CENTER), use_center);

	return true;
}

obs_properties_t * get_properties_for_overlay(void * data)
{
	InputSource * s = reinterpret_cast<InputSource*>(data);

	obs_properties_t *props = obs_properties_create();

	std::string img_path;
	std::string layout_path;
	std::string filter_img = util_file_filter(T_FILTER_IMAGE_FILES, "*.jpg *.png *.bmp");
	std::string filter_text = util_file_filter(T_FILTER_TEXT_FILES, "*.ini");

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

	// Mouse stuff
	obs_property_t * is_Mouse = obs_properties_add_bool(props, S_IS_MOUSE, T_IS_MOUSE);
	obs_property_set_modified_callback(is_Mouse, is_mouse_changed);

	obs_properties_add_int_slider(props, S_MOUSE_SENS, T_MOUSE_SENS, 1, 500, 1);

	obs_property_t *use_center = obs_properties_add_bool(props, S_MONITOR_USE_CENTER, T_MONITOR_USE_CENTER);
	obs_property_set_modified_callback(use_center, use_monitor_center_changed);

	obs_properties_add_int(props, S_MONITOR_H_CENTER, T_MONITOR_H_CENTER, -9999, 9999, 1);
	obs_properties_add_int(props, S_MONITOR_V_CENTER, T_MONITOR_V_CENTER, -9999, 9999, 1);
	obs_properties_add_int_slider(props, S_MOUSE_DEAD_ZONE, T_MOUSE_DEAD_ZONE, 0, 50, 1);

	// Gamepad stuff

	obs_property_t *is_controller = obs_properties_add_bool(props, S_IS_CONTROLLER, T_IS_CONTROLLER);
	obs_property_set_modified_callback(is_controller, is_controller_changed);
	obs_properties_add_int(props, S_CONTROLLER_ID, T_CONTROLLER_ID, 0, 3, 1);

#if HAVE_XINPUT // Linux only allows values 0 - 127
#define PAD_STICK_MAX_VAL 32767
	// which removes jittery input
	obs_properties_add_int_slider(props, S_CONTROLLER_L_DEAD_ZONE,
		T_CONROLLER_L_DEADZONE, 1, PAD_STICK_MAX_VAL - 1, 1);
	obs_properties_add_int_slider(props, S_CONTROLLER_R_DEAD_ZONE,
		T_CONROLLER_R_DEADZONE, 1, PAD_STICK_MAX_VAL - 1, 1);
#endif
	return props;
}

void register_overlay_source()
{
	// Input Overlay
	obs_source_info si = {};
	si.id = "input-overlay";
	si.type = OBS_SOURCE_TYPE_INPUT;
	si.output_flags = OBS_SOURCE_VIDEO;
	si.get_properties = get_properties_for_overlay;

	si.get_name = [](void*) { return obs_module_text("InputOverlay"); };
	si.create = [](obs_data_t *settings, obs_source_t *source)
	{ return (void*) new InputSource(source, settings);    };
	si.destroy = [](void *data) { delete reinterpret_cast<InputSource*>(data);    };
	si.get_width = [](void *data) { return reinterpret_cast<InputSource*>(data)->cx;    };
	si.get_height = [](void *data) { return reinterpret_cast<InputSource*>(data)->cy;    };

	si.get_defaults = [](obs_data_t *settings)
	{
		// NO-OP
	};

	si.update = [](void *data, obs_data_t *settings)
	{ reinterpret_cast<InputSource*>(data)->Update(settings); };
	si.video_tick = [](void *data, float seconds)
	{ reinterpret_cast<InputSource*>(data)->Tick(seconds); };
	si.video_render = [](void *data, gs_effect_t *effect)
	{ reinterpret_cast<InputSource*>(data)->Render(effect); };
	obs_register_source(&si);
}

}
