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

#include "gamepad_xinput.hpp"
#include "../../util/log.h"

namespace gamepad {
handle_xinput::handle_xinput(int8_t id) : handle(id)
{
	m_pad = xinput_fix::create();
}

handle_xinput::~handle_xinput()
{
	xinput_fix::free(m_pad);
}

void handle_xinput::load()
{
	unload();
	update();
	bdebug("Gamepad %i %s present", m_id, valid() ? "" : "not");
}

void handle_xinput::update()
{
	m_valid = xinput_fix::update(m_pad, m_id);
}

}
