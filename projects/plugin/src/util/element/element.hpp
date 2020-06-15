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

#include "graphics/graphics.h"
#include "graphics/vec2.h"
#include <QJsonObject>
#include <string>

typedef struct gs_image_file gs_image_file_t;

/**
 * Which data holder to read element
 * data from
 */
enum data_source {
	DS_NONE = -1,
	DS_DEFAULT,
	DS_GAMEPAD,
	DS_MOUSE_POS /* TODO: use this?*/
};

namespace sources {
class overlay_settings;
}

#ifdef _WIN32
enum element_type;
#else
#include <layout_constants.h>
#endif

class element_data {
public:
	explicit element_data(element_type type);
	virtual ~element_data() = default;

	element_type get_type() const;

	/* true if data should not me removed */
	virtual bool is_persistent() { return false; }

	/* used if is persistent
     * returns true if new data differed from old one and this input should
     * invoke an update for input history things like mouse or analog stick
     * movement return false to prevent spamming input history */
	virtual bool merge(element_data *other)
	{
		UNUSED_PARAMETER(other);
		/* NO-OP */
		return false;
	}

protected:
	element_type m_type;
};

class element {
public:
	virtual ~element() = default;

	element();

	element(element_type type);

	virtual void load(const QJsonObject &obj) = 0;

	virtual void draw(gs_effect_t *effect, gs_image_file_t *m_image, element_data *data,
					  sources::overlay_settings *settings) = 0;

	element_type get_type() const;

	uint16_t get_keycode() const;

	virtual data_source get_source();

protected:
	void read_mapping(const QJsonObject &obj);

	void read_pos(const QJsonObject &obj);

	vec2 m_pos = {};
	gs_rect m_mapping = {};

	element_type m_type;
	uint16_t m_keycode;
};
