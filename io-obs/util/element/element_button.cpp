/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <universailp@web.de>.
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

bool element_data_button::merge(element_data* other)
{
    bool result = false;
    if (other && other->get_type() == ET_BUTTON) {
        const auto other_btn = dynamic_cast<element_data_button*>(other);
        if (other_btn) {
            if (m_state == BS_RELEASED && other_btn->m_state == BS_PRESSED)
                result = true;
            m_state = other_btn->m_state;
        }
    }
    return result;
}

void element_button::load(const QJsonObject& obj)
{
    element_texture::load(obj);
    m_keycode = static_cast<uint16_t>(obj[CFG_KEY_CODE].toInt());
    m_pressed = m_mapping;
    m_pressed.y = m_mapping.y + m_mapping.cy + CFG_INNER_BORDER;
    /* Checks whether first 8 bits are equal */
    is_gamepad = (m_keycode >> 8) == (VC_PAD_MASK >> 8);
}

void element_button::draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data,
    sources::overlay_settings* settings)
{
    UNUSED_PARAMETER(settings);
    if (data) {
        const auto button = dynamic_cast<element_data_button*>(data);
        if (button) {
            if (button->get_state() == BS_PRESSED) {
                element_texture::draw(effect, image, &m_pressed);
            } else {
                element_texture::draw(effect, image, nullptr);
            }
        }
    } else {
        element_texture::draw(effect, image, nullptr);
    }
}
