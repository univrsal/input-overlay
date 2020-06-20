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

#include "scale_effect.hpp"

extern "C" {
#include <graphics/graphics.h>
}

scale_effect::scale_effect(const float duration, const float scale) : effect(duration), m_scale(0.f)
{
	m_delta = scale / duration;
}

scale_effect::scale_effect(const float duration, const float scale, const float start)
	: effect(duration), m_scale(start)
{
	m_delta = scale / duration;
}

void scale_effect::tick(const float seconds)
{
	effect::tick(seconds);
	m_scale += m_delta * seconds;
}

void scale_effect::render()
{
	gs_matrix_scale3f(m_scale, m_scale, m_scale);
}
