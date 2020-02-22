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

#include "translate_effect.hpp"

extern "C" {
#include <graphics/graphics.h>
}

translate_effect::translate_effect(const float duration, vec2& direction, vec2* target, const bool translate)
    : effect(duration)
    , m_direction(direction)
    , m_translate(translate)
{
    m_pos = target;
    m_original = *target;
}

void translate_effect::tick(const float seconds)
{
    effect::tick(seconds);
    m_pos->x = m_original.x;
    m_pos->y = m_original.y;
    vec2_mulf(m_pos, &m_direction, get_progress());
    vec2_add(m_pos, &m_original, m_pos);
}

void translate_effect::render()
{
    if (m_translate) {
        gs_matrix_push();
        gs_matrix_translate3f(m_pos->x, m_pos->y, 1.f);
        gs_matrix_pop();
    }
}

translate_effect::~translate_effect()
{
    /* Make sure new position is applied */
    m_pos->x = m_original.x;
    m_pos->y = m_original.y;
    vec2_add(m_pos, m_pos, &m_direction);
}

uint8_t translate_effect::get_priority()
{
    return 5;
}
