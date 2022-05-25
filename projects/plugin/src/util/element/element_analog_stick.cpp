/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2022 univrsal <uni@vrsal.xyz>.
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

#include "element_analog_stick.hpp"
#include <keycodes.h>

#include "src/sources/input_source.hpp"

void element_analog_stick::load(const QJsonObject &obj)
{
    element_texture::load(obj);
    m_side = static_cast<element_side>(obj[CFG_SIDE].toInt());
    m_radius = static_cast<uint8_t>(obj[CFG_STICK_RADIUS].toInt());
    m_keycode = VC_STICK_DATA;
    m_pressed = m_mapping;
    m_pressed.y = m_mapping.y + m_mapping.cy + CFG_INNER_BORDER;
}

void element_analog_stick::draw(gs_effect_t *effect, gs_image_file_t *image, sources::overlay_settings *settings)
{
    auto pos = m_pos;
    gs_rect *temp;

    if (m_side == element_side::LEFT) {
        pos.y += ((settings->data.gamepad_axis[gamepad::axis::LEFT_STICK_Y] * 2) - 1) * m_radius;
        pos.x += ((settings->data.gamepad_axis[gamepad::axis::LEFT_STICK_X] * 2) - 1) * m_radius;
        temp = settings->data.gamepad_buttons[gamepad::button::L_THUMB] ? &m_pressed : &m_mapping;
    } else {
        pos.y += ((settings->data.gamepad_axis[gamepad::axis::RIGHT_STICK_Y] * 2) - 1) * m_radius;
        pos.x += ((settings->data.gamepad_axis[gamepad::axis::RIGHT_STICK_X] * 2) - 1) * m_radius;
        temp = settings->data.gamepad_buttons[gamepad::button::R_THUMB] ? &m_pressed : &m_mapping;
    }
    element_texture::draw(effect, image, temp, &pos);
}
