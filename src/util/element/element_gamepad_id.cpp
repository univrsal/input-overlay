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

#include "element_gamepad_id.hpp"
#include "../../sources/input_source.hpp"
#include "../../hook/sdl_gamepad.hpp"

element_gamepad_id::element_gamepad_id() : element_texture(ET_GAMEPAD_ID), m_mappings{} {}

void element_gamepad_id::load(const QJsonObject &obj)
{
    element_texture::load(obj);
    auto i = 1;
    for (auto &map : m_mappings) {
        map = m_mapping;
        map.x += i++ * (m_mapping.cx + CFG_INNER_BORDER);
    }
}

void element_gamepad_id::draw(gs_effect_t *effect, gs_image_file_t *image, sources::overlay_settings *settings)
{
    if (settings->gamepad || settings->remote_input_data) {
        if (settings->is_pad_button_pressed(SDL_CONTROLLER_BUTTON_GUIDE))
            element_texture::draw(effect, image, &m_mappings[ID_PRESSED]);
        int index = SDL_min(settings->gamepad_index, 2);
        element_texture::draw(effect, image, &m_mappings[index]);
    }
}
