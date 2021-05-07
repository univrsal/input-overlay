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

#include "element_gamepad_id.hpp"
#include "element_button.hpp"
#include "../../sources/input_source.hpp"
#include "../../hook/gamepad_hook_helper.hpp"
#include <libgamepad.hpp>

element_gamepad_id::element_gamepad_id() : element_texture(ET_GAMEPAD_ID), m_mappings{}
{
    m_keycode = gamepad::button::GUIDE;
}

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
    if (settings->data.gamepad_buttons[m_keycode])
        element_texture::draw(effect, image, &m_mappings[ID_PRESSED]);

    if (settings->gamepad) {
        if (settings->gamepad->is_valid()) {
            int index = settings->gamepad->get_index() < 4 ? settings->gamepad->get_index() : 0;
            element_texture::draw(effect, image, &m_mappings[index]);
        }
    }
}
