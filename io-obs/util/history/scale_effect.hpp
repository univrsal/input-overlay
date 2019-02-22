/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include "effect.hpp"

class scale_effect
    : public effect
{
    float m_scale;
    float m_delta;
public:
    scale_effect(float duration, float scale);
    scale_effect(float duration, float scale, float start);

    void tick(float seconds) override;
    void render() override;
};
