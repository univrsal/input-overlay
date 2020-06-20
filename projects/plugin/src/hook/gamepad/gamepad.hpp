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

#pragma once
#include <QString>
#include <stdint.h>
#include <keycodes.h>
#include <string>

namespace gamepad {

extern code all_codes[13];
extern uint16_t to_vc(code c);

class handle {
protected:
	int8_t m_id = -1;
	bool m_valid = false;
	QString m_device_name;

public:
	handle(int8_t id);
	~handle();

	virtual void update();
	virtual void load() = 0;
	virtual void unload();

	bool valid() const;
	int8_t id() const;
	const QString &get_name() const { return m_device_name; }
};

}
