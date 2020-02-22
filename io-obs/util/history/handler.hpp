/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <universailp@web.de>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#pragma once
#include <obs-module.h>

class input_entry;

namespace sources {
struct history_settings;
}

class handler {
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
