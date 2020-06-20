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

#include "gamepad_linux.hpp"
#include "../../util/log.h"
#include "../../util/obs_util.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace gamepad {
handle_linux::handle_linux(int8_t id) : handle(id)
{
	load();
}

handle_linux::~handle_linux()
{
	unload();
}

void handle_linux::load()
{
	unload();
	m_path = "/dev/input/js";
	m_path.append(std::to_string(m_id));
	m_device_id = open(m_path.c_str(), O_RDONLY | O_NONBLOCK);
	m_valid = m_device_id >= 0;

	if (m_valid) {
		char buf[64];

		if (ioctl(m_device_id, JSIOCGNAME(64), &buf) != -1)
			m_device_name = utf8_to_qt(buf);
	}

	bdebug("Gamepad (%s) %i %s present", qt_to_utf8(m_device_name), m_id, valid() ? "" : "not");
}

void handle_linux::unload()
{
	close(m_device_id);
}

int handle_linux::read_event()
{
	ssize_t bytes;
	bytes = read(m_device_id, &m_event, sizeof(m_event));
	if (bytes == sizeof(m_event))
		return 0;

	/* Error, could not read full event. */
	return -1;
}

js_event *handle_linux::event()
{
	return &m_event;
}
}
