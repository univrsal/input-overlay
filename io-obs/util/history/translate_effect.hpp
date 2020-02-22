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

#include "effect.hpp"
#include "graphics/vec2.h"
#include "graphics/vec3.h"

class translate_effect : public effect {
    vec2 m_direction, m_original; /* Original contains a copy of m_pos, which is used for the modification */
    vec2* m_pos = nullptr; /* This vector will be modified over time by tick() */
    /* If set to false this effect will only modify the position vector
     * without performing any matrix translation
     */
    bool m_translate = true;

public:
    translate_effect(float duration, vec2& direction, vec2* target, bool translate);
    ~translate_effect() override;

    void tick(float seconds) override;

    void render() override;

    uint8_t get_priority() override;
};
