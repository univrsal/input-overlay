/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#include "scale_effect.hpp"

extern "C" {
#include <graphics/graphics.h>
}

scale_effect::scale_effect(const float duration, const float scale) : effect(duration), m_scale(0.f)
{
    m_delta = scale / duration;
}

scale_effect::scale_effect(const float duration, const float scale, const float start) : effect(duration),
                                                                                         m_scale(start)
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
    gs_matrix_push();
    gs_matrix_scale3f(m_scale, m_scale, 1.f);
    gs_matrix_pop();
}
