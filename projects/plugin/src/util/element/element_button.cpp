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

#include "element_button.hpp"
#include <keycodes.h>

#include "src/sources/input_source.hpp"

void element_button::load(const QJsonObject &obj)
{
    element_texture::load(obj);
    m_keycode = static_cast<uint16_t>(obj[CFG_KEY_CODE].toInt());
    m_pressed = m_mapping;
    m_pressed.y = m_mapping.y + m_mapping.cy + CFG_INNER_BORDER;
}

void element_button::draw(gs_effect_t *effect, gs_image_file_t *image, sources::overlay_settings *settings)
{
    /* TODO: this should only check either mouse buttons,
     * keyboard keys or gamepad buttons
     */
    if (settings->data.gamepad_buttons[m_keycode] || settings->data.keyboard[m_keycode] ||
        settings->data.mouse[m_keycode]) {
        element_texture::draw(effect, image, &m_pressed);
    } else {
        element_texture::draw(effect, image, nullptr);
    }
}
