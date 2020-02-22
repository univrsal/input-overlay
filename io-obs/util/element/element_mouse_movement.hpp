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

#include "element_texture.hpp"
#include <layout_constants.h>

/* Contains all information about mouse movement,
 * button clicks and scroll motion
 */
class element_data_mouse_pos : public element_data {
public:
    element_data_mouse_pos(int16_t x = 0, int16_t y = 0);

    bool is_persistent() override;
    bool merge(element_data* other) override;
    float get_mouse_angle(sources::overlay_settings* settings);
    void get_mouse_offset(sources::overlay_settings* settings, const vec2& center, vec2& out, uint8_t radius) const;
    int16_t get_mouse_x() const;
    int16_t get_mouse_y() const;

private:
    int16_t m_x {}, m_y {};
    int16_t m_last_x {}, m_last_y {};
    float m_old_angle {};
};

class element_mouse_movement : public element_texture {
public:
    element_mouse_movement();

    void load(const QJsonObject& obj) override;

    void
    draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data, sources::overlay_settings* settings) override;

    data_source get_source() override
    {
        return DS_MOUSE_POS;
    }

private:
    mouse_movement m_movement_type;
    vec2 m_offset_pos = {};
    uint8_t m_radius = 0;
};
