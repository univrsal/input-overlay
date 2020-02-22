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

#include "element_dpad.hpp"
#include "../../../ccl/ccl.hpp"
#include "../../sources/input_source.hpp"
#include <keycodes.h>

element_dpad::element_dpad()
    : element_texture(ET_DPAD_STICK)
{
}

void element_dpad::load(const QJsonObject& obj)
{
    element_texture::load(obj);
    auto i = 1;
    for (auto& map : m_mappings) {
        map = m_mapping;
        map.x += i * (m_mapping.cx + CFG_INNER_BORDER);
        i++;
    }
    m_keycode = VC_DPAD_DATA;
}

void element_dpad::draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data,
    sources::overlay_settings* settings)
{
    const auto d = dynamic_cast<element_data_dpad*>(data);

    if (d && d->get_direction() != DT_CENTER) {
        /* Enum starts at one (Center doesn't count)*/
        const auto map = &m_mappings[(int)d->get_direction() - 1];
        element_texture::draw(effect, image, map);
    } else {
        element_texture::draw(effect, image, nullptr);
    }
    UNUSED_PARAMETER(settings);
}

data_source element_dpad::get_source()
{
    return DS_GAMEPAD;
}

element_data_dpad::element_data_dpad(const dpad_direction a, const dpad_direction b)
    : element_data(ET_DPAD_STICK)
{
    m_direction = (int)a | (int)b;
    m_state = BS_RELEASED;
}

element_data_dpad::element_data_dpad(const dpad_direction d, const button_state state)
    : element_data(ET_DPAD_STICK)
{
    m_direction = (int)d;
    m_state = state;
}

bool element_data_dpad::is_persistent()
{
    return true;
}

bool element_data_dpad::merge(element_data* other)
{
    bool result = false;
    const auto d = dynamic_cast<element_data_dpad*>(other);
#ifdef _WIN32
    if (d) {
        result = m_direction != d->m_direction;
        m_direction = d->m_direction;
    }
#else
    if (d) {
        result = m_direction != d->m_direction;
        if (d->get_state() == BS_PRESSED) {
            m_direction |= d->m_direction;
        } else {
            m_direction &= ~d->m_direction;
        }
    }
#endif /* !WINDOWS*/
    return result;
}

dpad_texture element_data_dpad::get_direction() const
{
    if (m_direction & DD_UP && m_direction & DD_LEFT)
        return DT_TOP_LEFT;
    else if (m_direction & DD_UP && m_direction & DD_RIGHT)
        return DT_TOP_RIGHT;
    else if (m_direction & DD_DOWN && m_direction & DD_LEFT)
        return DT_BOTTOM_LEFT;
    else if (m_direction & DD_DOWN && m_direction & DD_RIGHT)
        return DT_BOTTOM_RIGHT;
    else if (m_direction & DT_UP)
        return DT_UP;
    else if (m_direction & DD_DOWN)
        return DT_DOWN;
    else if (m_direction & DD_LEFT)
        return DT_LEFT;
    else
        return DT_RIGHT;
}

button_state element_data_dpad::get_state() const
{
    return m_state;
}
