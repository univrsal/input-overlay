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

#include "util/util.hpp"
#include "effect.hpp"

effect::effect(const float duration)
{
	m_duration = duration;
	m_time = 0.f;
}

void effect::tick(const float seconds)
{
	m_time += seconds;
}

bool effect::done()
{
	return m_time >= m_duration;
}

float effect::get_progress() const
{
	return UTIL_MIN(m_time / m_duration, 1.f);
}

unsigned char effect::get_priority()
{
	return 0;
}
