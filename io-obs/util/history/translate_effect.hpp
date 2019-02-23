/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include "effect.hpp"
#include "graphics/vec3.h"
#include "graphics/vec2.h"

class translate_effect
    : public effect
{
    vec2 m_direction;
    vec2* m_pos = nullptr; /* This vector will be modified over time by tick() */
    /* If set to false this effect will only modify the position vector
     * without performing any matrix translation
     */
    bool m_translate = true; 
public:
    translate_effect(float duration, vec2& direction, vec2* target, bool translate);

    void tick(float seconds) override;
    void render() override;
};
