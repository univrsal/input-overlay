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

#include "gamepad_binding.hpp"
#include "util/settings.h"
#include "util/log.h"
#include <keycodes.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QJsonArray>
#include <obs-module.h>

namespace gamepad {
std::vector<bindings> loaded_bindings;

std::vector<bind> bindings::m_defaults =
	{{S_BINDING_A, "txt_a", VC_PAD_A, false},
	{S_BINDING_B, "txt_b", VC_PAD_B, false},
	{S_BINDING_X, "txt_x", VC_PAD_X, false},
	{S_BINDING_Y, "txt_y", VC_PAD_Y, false},
	{S_BINDING_GUIDE, "txt_guide", VC_PAD_GUIDE, false},
	{S_BINDING_LT, "txt_lt", VC_PAD_LT, true},
	{S_BINDING_RT, "txt_rt", VC_PAD_RT, true},
	{S_BINDING_RB, "txt_rb", VC_PAD_RB, false},
	{S_BINDING_START, "txt_start", VC_PAD_START, false},
	{S_BINDING_BACK, "txt_back", VC_PAD_BACK, false},
	{S_BINDING_DPAD_UP, "txt_dpad_up", VC_PAD_DPAD_UP, false},
	{S_BINDING_DPAD_DOWN, "txt_dpad_down", VC_PAD_DPAD_DOWN, false},
	{S_BINDING_DPAD_LEFT, "txt_dpad_left", VC_PAD_DPAD_LEFT, false},
	{S_BINDING_LB, "txt_lb", VC_PAD_LB, false},
	{S_BINDING_DPAD_RIGHT, "txt_dpad_right", VC_PAD_DPAD_RIGHT, false},
	{S_BINDING_ANALOG_L, "txt_analog_left", VC_PAD_L_ANALOG, false},
	{S_BINDING_ANALOG_R, "txt_analog_right", VC_PAD_R_ANALOG, false},
	{S_BINDING_ANALOG_LX, "txt_lx", VC_PAD_LX, true},
	{S_BINDING_ANALOG_LY, "txt_ly", VC_PAD_LX, true},
	{S_BINDING_ANALOG_RX, "txt_rx", VC_PAD_RX, true},
	{S_BINDING_ANALOG_RY, "txt_ry", VC_PAD_RY, true}};

bindings::bindings()
{
	
}

void load_bindings()
{
	
}

void save_bindings()
{
	auto* path = obs_module_file("bindings");
	QFile f(path);
	QJsonParseError err;
	bfree(path);
	if (!f.open(QIODevice::ReadOnly)) {
		bwarn("Couldn't load gamepad bindings");
		return;
	}

	const auto doc = QJsonDocument::fromJson(f.readAll(), &err);

	if (err.error == QJsonParseError::NoError && doc.isArray()) {
		auto bindings = doc.array();
		for (auto e : bindings) {
			if (!e.isObject())
				continue;
			auto obj = e.toObject();
			
		}
	} else {
		bwarn("Json parse error: %s", err.errorString());
	}
}

}
