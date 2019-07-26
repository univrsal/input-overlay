/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

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
