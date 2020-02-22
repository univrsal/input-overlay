/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <universailp@web.de>.
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

#include "overlay.hpp"
#include "../../ccl/ccl.hpp"
#include "../sources/input_source.hpp"
#include "config.hpp"
#include "element/element.hpp"
#include "element/element_analog_stick.hpp"
#include "element/element_button.hpp"
#include "element/element_data_holder.hpp"
#include "element/element_dpad.hpp"
#include "element/element_gamepad_id.hpp"
#include "element/element_mouse_movement.hpp"
#include "element/element_mouse_wheel.hpp"
#include "element/element_trigger.hpp"
#include "gui/io_settings_dialog.hpp"
#include "log.h"
#include "network/io_server.hpp"
#include "network/remote_connection.hpp"
#include "obs_util.hpp"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <layout_constants.h>
extern "C" {
#include <graphics/image-file.h>
}

namespace sources {
class overlay_settings;
}

overlay::~overlay()
{
	unload();
}

overlay::overlay(sources::overlay_settings *settings)
{
	m_settings = settings;
	m_is_loaded = load();
}

bool overlay::load()
{
	unload();
	const auto image_loaded = load_texture();
	m_is_loaded = image_loaded && load_cfg();

	if (!m_is_loaded) {
		m_settings->gamepad = 0;
		if (!image_loaded) {
			m_settings->cx = 100; /* Default size */
			m_settings->cy = 100;
		}
	}

	return m_is_loaded;
}

void overlay::unload()
{
	unload_texture();
	unload_elements();
	m_data.clear();
	m_settings->gamepad = 0;
	m_settings->cx = 100;
	m_settings->cy = 100;
}

bool overlay::load_cfg()
{
	if (!m_settings || m_settings->layout_file.empty())
		return false;

	QFile file(m_settings->layout_file.c_str());

	if (!file.open(QIODevice::ReadOnly)) {
		blog(LOG_ERROR, "[input-overlay] couldn't open config file");
		return false;
	}

	QJsonParseError err;
	auto cfg_doc = QJsonDocument::fromJson(file.readAll(), &err);
	auto cfg_obj = cfg_doc.object();
	auto flag = true;

	if (err.error == QJsonParseError::NoError) {
		m_settings->cx = static_cast<uint32_t>(cfg_obj[CFG_TOTAL_WIDTH].toInt());
		m_settings->cy = static_cast<uint32_t>(cfg_obj[CFG_TOTAL_HEIGHT].toInt());
		m_settings->layout_flags = static_cast<uint8_t>(cfg_obj[CFG_FLAGS].toInt());

		const auto debug_mode = cfg_obj[CFG_DEBUG_FLAG].toBool();

#ifndef _DEBUG
		if (debug_mode) {
#else
		{
#endif
			blog(LOG_INFO, "[input-overlay] Started loading of %s", m_settings->layout_file.c_str());
		}

		auto arr = cfg_obj[CFG_ELEMENTS].toArray();

		for (const auto element : arr)
			load_element(element.toObject(), debug_mode);
	} else {
		berr("Couldn't load layout from %s. Error: %s", m_settings->layout_file.c_str(), qt_to_utf8(err.errorString()));
	}

	return flag;
}

bool overlay::load_texture()
{
	if (!m_settings || m_settings->image_file.empty())
		return false;

	auto flag = true;

	if (m_image == nullptr) {
		m_image = new gs_image_file_t();
	}

	gs_image_file_init(m_image, m_settings->image_file.c_str());

	obs_enter_graphics();
	gs_image_file_init_texture(m_image);
	obs_leave_graphics();

	if (!m_image->loaded) {
		bwarn("Error: failed to load texture %s", m_settings->image_file.c_str());
		flag = false;
	} else {
		m_settings->cx = m_image->cx;
		m_settings->cy = m_image->cy;
	}

	return flag;
}

void overlay::unload_texture() const
{
	obs_enter_graphics();
	gs_image_file_free(m_image);
	obs_leave_graphics();
}

void overlay::unload_elements()
{
	m_elements.clear();
}

void overlay::draw(gs_effect_t *effect)
{
	if (m_is_loaded) {
		for (auto const &element : m_elements) {
			const auto data = m_data[element->get_keycode()].get();
			element->draw(effect, m_image, data, m_settings);
		}
	}
}

void overlay::refresh_data()
{
	/* This copies over necessary element data information
     * to make sure the overlay always has data available to
     * draw the overlay. If the data was directly accessed in the render
     * method, the overlay can start to flicker if the frame is rendered
     * while the data is currently inaccessible, because it is being written
     * to by the input thread, resulting in all buttons being unpressed
     */
	if (io_config::io_window_filters.input_blocked())
		return;
	element_data_holder *source = nullptr;
	std::lock_guard<std::mutex> lck1(hook::mutex);
	std::lock_guard<std::mutex> lck2(network::mutex);

	if (hook::data_initialized || network::network_flag) {
		if (network::server_instance && m_settings->selected_source > 0) {
			source = network::server_instance->get_client(m_settings->selected_source - 1)->get_data();
		} else {
			source = hook::input_data;
		}
	}

	if (source) {
		for (auto const &element : m_elements) {
			element_data *data = nullptr;

			if (m_data[element->get_keycode()] != nullptr) {
				switch (element->get_source()) {
				case DS_GAMEPAD:
					data = source->get_by_gamepad(m_settings->gamepad, element->get_keycode());
					break;
				default:
				case DS_MOUSE_POS:
				case DS_DEFAULT:
					data = source->get_by_code(element->get_keycode());
					break;
				}
				m_data[element->get_keycode()]->merge(data);
			}
		}
	}
}

void overlay::load_element(const QJsonObject &obj, const bool debug)
{
	const auto type = obj[CFG_TYPE].toInt();
	element *new_element = nullptr;
	element_data *data = nullptr;

	switch (type) {
	case ET_TEXTURE:
		new_element = new element_texture;
		break;
	case ET_GAMEPAD_ID:
		new_element = new element_gamepad_id;
		/* Fallthrough */
	case ET_BUTTON:
		data = new element_data_button;
		new_element = new element_button;
		break;
	case ET_WHEEL:
		new_element = new element_wheel;
		data = new element_data_wheel;
		break;
	case ET_TRIGGER:
		new_element = new element_trigger;
		data = new element_data_trigger;
		break;
	case ET_ANALOG_STICK:
		new_element = new element_analog_stick;
		data = new element_data_analog_stick;
		break;
	case ET_DPAD_STICK:
		new_element = new element_dpad;
		data = new element_data_dpad;
		break;
	case ET_MOUSE_STATS:
		new_element = new element_mouse_movement;
		data = new element_data_mouse_pos;
		break;
	default:
		if (debug) {
			binfo("Invalid element type %i for %s", type, qt_to_utf8(obj[CFG_ID].toString()));
		}
	}

	if (new_element) {
		new_element->load(obj);
		m_elements.emplace_back(new_element);

		if (data)
			m_data[new_element->get_keycode()] = std::unique_ptr<element_data>(data);

#ifndef _DEBUG
		if (debug) {
#else
		{
#endif
			binfo("Type: %14s, KEYCODE: 0x%04X ID: %s", element_type_to_string(static_cast<element_type>(type)),
				  new_element->get_keycode(), qt_to_utf8(obj[CFG_ID].toString()));
		}
	}
}

const char *overlay::element_type_to_string(const element_type t)
{
	switch (t) {
	case ET_TEXTURE:
		return "Texture";
	case ET_BUTTON:
		return "Button";
	case ET_ANALOG_STICK:
		return "Analog stick";
	case ET_WHEEL:
		return "Scroll wheel";
	case ET_MOUSE_STATS:
		return "Mouse movement";
	case ET_TRIGGER:
		return "Trigger";
	case ET_GAMEPAD_ID:
		return "Gamepad ID";
	case ET_DPAD_STICK:
		return "DPad";
	case ET_INVALID:
		return "Invalid";
	}
}
