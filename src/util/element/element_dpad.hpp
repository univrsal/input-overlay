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

class element_dpad : public element_texture {
public:
    element_dpad();

    void load(const QJsonObject &obj) override;

    void draw(gs_effect_t *effect, gs_image_file_t *image, sources::overlay_settings *settings) override;

    enum {
        TEXTURE_LEFT,
        TEXTURE_RIGHT,
        TEXTURE_UP,
        TEXTURE_DOWN,
        TEXTURE_TOP_LEFT,
        TEXTURE_TOP_RIGHT,
        TEXTURE_BOTTOM_LEFT,
        TEXTURE_BOTTOM_RIGHT
    };

private:
    /* Center is in m_mapping */
    gs_rect m_mappings[8]; /* Left, Right, Up, Down, Top Left, Top Right, Bottom Left, Bottom Right */
};
