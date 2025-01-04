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

enum stick_data_type {
    SD_BOTH,
    SD_PRESSED_STATE_LEFT,
    SD_PRESSED_STATE_RIGHT,
    SD_LEFT_X,
    SD_LEFT_Y,
    SD_RIGHT_X,
    SD_RIGHT_Y
};

class element_analog_stick : public element_texture {
public:
    element_analog_stick() : element_texture(ET_ANALOG_STICK), m_side() {}

    void load(const QJsonObject &obj) override;

    void draw(gs_effect_t *effect, gs_image_file_t *image, sources::overlay_settings *settings) override;

private:
    void calc_position(vec2 &v);

    gs_rect m_pressed{};
    element_side m_side;
    int32_t m_radius = 0;
};
