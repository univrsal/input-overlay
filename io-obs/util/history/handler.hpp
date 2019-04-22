/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once
#include <obs-module.h>

class input_entry;

namespace sources
{
    struct history_settings;
}

class handler
{
protected:
    sources::history_settings* m_settings = nullptr;
public:
    explicit handler(sources::history_settings* settings)
    {
        m_settings = settings;
    }

    virtual ~handler() = default;

    virtual void update() = 0;

    virtual void tick(float seconds) = 0;

    virtual void swap(input_entry& current) = 0;

    virtual void render(const gs_effect_t* effect) = 0;

    virtual void clear() = 0;
};


