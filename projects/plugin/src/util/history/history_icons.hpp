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

#pragma once

#include <QMap>
#include <QString>

extern "C" {
#include <graphics/image-file.h>
}

struct icon {
	uint16_t u, v, cx, cy;
};

class input_entry;

class history_icons {
	bool m_loaded = false;
	uint16_t m_icon_count = 0;
	uint16_t m_icon_max_w = 0;
	uint16_t m_icon_max_h = 0;
	QMap<uint16_t, icon> m_icons;
	gs_image_file_t *m_icon_texture = nullptr;

	void unload_texture();

public:
	~history_icons();

	void load_from_file(const QString &cfg, const QString &img);

	void draw(uint16_t vc, vec2 *pos, input_entry *parent);

	gs_image_file_t *image_file();

	uint16_t get_w() const { return m_icon_max_w; }

	uint16_t get_h() const { return m_icon_max_h; }

	bool is_loaded() const { return m_loaded; }
};
