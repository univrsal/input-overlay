/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "translate_effect.hpp"

extern "C" {
#include <graphics/graphics.h>
}

translate_effect::translate_effect(const float duration, vec3& direction)
    : effect(duration), m_direction(direction), m_total()
{
    /* NO-OP */
}

void translate_effect::tick(const float seconds)
{
    effect::tick(seconds);
    vec3_mulf(&m_total, &m_direction, get_progress());
}

void translate_effect::render()
{
    gs_matrix_translate(&m_total);
}
