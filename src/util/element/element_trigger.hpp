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

class element_trigger : public element_texture {
public:
    element_trigger();

    void load(const QJsonObject &obj) override;

    void draw(gs_effect_t *effect, gs_image_file_t *image, sources::overlay_settings *settings) override;

private:
    void calculate_mapping(gs_rect *pressed, vec2 *pos, float progress) const;
    gs_rect m_pressed;
    element_side m_side;
    direction m_direction;
    bool m_button_mode = false;
};
