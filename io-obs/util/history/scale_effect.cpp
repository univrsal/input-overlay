/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "scale_effect.hpp"
#include "graphics/vec3.h"

extern "C" {
#include <graphics/graphics.h>
}

scale_effect::scale_effect(const float duration, const float scale)
    : effect(duration), m_scale(0.f)
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
