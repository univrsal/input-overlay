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

#include <graphics/graphics.h>
#include <graphics/vec2.h>
#include <QJsonObject>
#include <layout_constants.h>

typedef struct gs_image_file gs_image_file_t;
typedef struct gs_image_file4 gs_image_file4_t;

namespace sources {
class overlay_settings;
}

class element {
public:
    virtual ~element() = default;

    element();

    element(element_type type);

    virtual void load(const QJsonObject &obj) = 0;

    virtual void draw(gs_effect_t *effect, gs_image_file_t *m_image, sources::overlay_settings *settings) = 0;

    element_type get_type() const;

    uint16_t get_keycode() const;

    virtual void tick(float, sources::overlay_settings *) {}

protected:
    void read_mapping(const QJsonObject &obj);

    void read_pos(const QJsonObject &obj);

    vec2 m_pos = {};
    gs_rect m_mapping = {};

    element_type m_type;
    uint16_t m_keycode;
};
