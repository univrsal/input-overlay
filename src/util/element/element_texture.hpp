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

#include "element.hpp"

class element_texture : public element {
public:
    element_texture();

    explicit element_texture(element_type type);

    void load(const QJsonObject &obj) override;
    void draw(gs_effect_t *effect, gs_image_file_t *image, sources::overlay_settings *settings) override;
    void draw(gs_effect_t *effect, gs_image_file_t *image, const gs_rect *rect) const;
    static void draw(gs_effect_t *effect, gs_image_file_t *image, const gs_rect *rect, const vec2 *pos);
    static void draw(gs_effect *effect, gs_image_file_t *image, const gs_rect *rect, const vec2 *pos, float angle);
};
