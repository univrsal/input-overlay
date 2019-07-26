/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once
#define EFFECT_MAX_PRIORITY 10
class effect
{
    float m_duration;
    float m_time;
public:
    effect(float duration);

    virtual ~effect() = default;

    virtual void tick(float seconds);

    virtual void render() = 0;

    virtual bool done();

    float get_progress() const;

    virtual unsigned char get_priority();
};
