/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

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
    return m_time / m_duration;
}
