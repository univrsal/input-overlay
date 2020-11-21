/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
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

#include "history_icons.hpp"
#include "input_entry.hpp"
#include "../obs_util.hpp"
#include "../log.h"
#include <QJsonObject>
#include <QJsonArray>
#include <obs-module.h>

void history_icons::unload_texture()
{
	if (m_icon_texture) {
		obs_enter_graphics();
		gs_image_file_free(m_icon_texture);
		m_icon_texture = nullptr;
		obs_leave_graphics();
	}
	m_loaded = false;
}

history_icons::~history_icons()
{
	unload_texture();
}

uint16_t util_read_hex(QString &l)
{
	uint16_t tmp;
	bool ok = true;
	tmp = static_cast<uint16_t>(l.toInt(&ok, 16));
	if (ok)
		return tmp;
	else
		berr("Converting hex %s", qt_to_utf8(l));

	return 0;
}

void history_icons::load_from_file(const QString &cfg, const QString &img)
{
	unload_texture();
	if (cfg.isEmpty() || img.isEmpty())
		return;
	m_icon_texture = new gs_image_file_t();

	gs_image_file_init(m_icon_texture, qt_to_utf8(img));

	obs_enter_graphics();
	gs_image_file_init_texture(m_icon_texture);
	obs_leave_graphics();

	if (!m_icon_texture->loaded) {
		berr("Error: Failed to load key icon image from %s", qt_to_utf8(img));
		unload_texture();
		return;
	}

	auto cfg_loaded = false;
	QJsonDocument doc;

	if (util_open_json(cfg, doc)) {
		auto obj = doc.object();
		cfg_loaded = true;
		m_icon_count = static_cast<uint16_t>(obj["icon_count"].toInt());
		auto icon_def_w = obj["icon_default_w"].toInt();
		auto icon_def_h = obj["icon_default_h"].toInt();
		bool debug = false;
		if (obj["debug"].isBool())
			debug = obj["debug"].toBool();
		const auto arr = obj["icons"].toArray();

		if (arr.isEmpty()) {
			berr("No icon data in array");
		} else {
			for (const auto val : arr) {
				const auto obj = val.toObject();
				icon ico{};
				const auto vc = static_cast<uint16_t>(obj["code"].toInt());
				if (debug)
					bdebug("Loaded icon with code 0x%X", vc);

				ico.u = static_cast<uint16_t>(obj["u"].toInt());
				ico.v = static_cast<uint16_t>(obj["v"].toInt());
				ico.cx = static_cast<uint16_t>(obj["cx"].toInt(icon_def_w));
				ico.cy = static_cast<uint16_t>(obj["cy"].toInt(icon_def_h));

				if (ico.cx > m_icon_max_w)
					m_icon_max_w = ico.cx;
				if (ico.cy > m_icon_max_h)
					m_icon_max_h = ico.cy;

				m_icons[vc] = ico;
			}
			m_icon_count = static_cast<uint16_t>(arr.size());
		}
	}
	m_loaded = cfg_loaded && m_icon_texture->loaded;
}

void history_icons::draw(uint16_t vc, vec2 *pos, input_entry *parent)
{
	if (m_icons.find(vc) != m_icons.end()) {
		auto icon = m_icons[vc];
		gs_matrix_push();
		gs_matrix_translate3f(pos->x, pos->y, 0.f);
		parent->render_effects();
		gs_draw_sprite_subregion(m_icon_texture->texture, 0, icon.u, icon.v, icon.cx + 1, icon.cy + 1);
		gs_matrix_pop();
	}
}

gs_image_file_t *history_icons::image_file()
{
	return m_icon_texture;
}
