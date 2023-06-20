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

#define ID_PRESSED 3

class element_gamepad_id : public element_texture {
public:
    element_gamepad_id();

    void load(const QJsonObject &obj) override;

    void draw(gs_effect_t *effect, gs_image_file_t *image, sources::overlay_settings *settings) override;

private:
    /* 0 - 2 => Player 2 to 4 (Player 1 is default)
     * 3     => Middle pressed down
     */
    gs_rect m_mappings[4];
};
