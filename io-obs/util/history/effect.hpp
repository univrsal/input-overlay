/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

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
};
