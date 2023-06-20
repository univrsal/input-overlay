/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.xyz/alex/input-overlay
 * Copyright 2023 univrsal <uni@vrsal.xyz>.
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

#include "element_texture.hpp"
#include <layout_constants.h>

class element_mouse_movement : public element_texture {
public:
    element_mouse_movement();

    void load(const QJsonObject &obj) override;

    void draw(gs_effect_t *effect, gs_image_file_t *image, sources::overlay_settings *settings) override;

    void tick(float seconds, sources::overlay_settings *settings) override;

private:
    float get_mouse_angle(sources::overlay_settings *settings);
    void get_mouse_offset(sources::overlay_settings *settings, const vec2 &center, vec2 &out,
                          const uint8_t radius) const;
    mouse_movement m_movement_type;
    vec2 m_offset_pos = {};
    uint8_t m_radius = 0;
    int m_last_x{}, m_last_y{};
    float m_angle{}, m_last_angle = 0.0f;
};
