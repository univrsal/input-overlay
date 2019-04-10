/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "effect.hpp"

class scale_effect : public effect
{
    float m_scale;
    float m_delta;
public:
    scale_effect(float duration, float scale);

    scale_effect(float duration, float scale, float start);

    void tick(float seconds) override;

    void render() override;
};
