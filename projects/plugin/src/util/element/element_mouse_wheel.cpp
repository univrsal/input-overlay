/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2021 univrsal <uni@vrsal.de>.
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

#include "element_mouse_wheel.hpp"
#include "src/sources/input_source.hpp"
#include <keycodes.h>

element_wheel::element_wheel() : element_texture(ET_WHEEL), m_mappings{}
{
    /* NO-OP */
}

void element_wheel::load(const QJsonObject &obj)
{
    element_texture::load(obj);
    auto i = 1;
    for (auto &map : m_mappings) {
        map = m_mapping;
        map.x += i * (m_mapping.cx + CFG_INNER_BORDER);
        i++;
    }
}

void element_wheel::draw(gs_effect_t *effect, gs_image_file_t *image, sources::overlay_settings *settings)
{
    if (settings->data.mouse[MOUSE_BUTTON3])
        element_texture::draw(effect, image, &m_mappings[WHEEL_MAP_MIDDLE]);
    switch (settings->data.last_wheel_event.rotation) {
    case WHEEL_UP:
        element_texture::draw(effect, image, &m_mappings[WHEEL_MAP_UP]);
        break;
    case WHEEL_DOWN:
        element_texture::draw(effect, image, &m_mappings[WHEEL_MAP_DOWN]);
        break;
    default:;
    }

    element_texture::draw(effect, image, settings);
}
