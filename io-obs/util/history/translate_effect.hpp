/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include "effect.hpp"
#include "graphics/vec3.h"

class translate_effect
    : public effect
{
    vec3 m_direction;
    vec3 m_total;
public:
    translate_effect(float duration, vec3& direction);

    void tick(float seconds) override;
    void render() override;
};
