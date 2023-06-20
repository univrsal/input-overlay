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

#include "element.hpp"
#include <QJsonArray>

element::element() : m_keycode(0)
{
    m_type = ET_INVALID;
}

element::element(const element_type type) : m_type(type), m_keycode(0) {}

element_type element::get_type() const
{
    return m_type;
}

uint16_t element::get_keycode() const
{
    return m_keycode;
}

void element::read_mapping(const QJsonObject &obj)
{
    auto map = obj[CFG_MAPPING].toArray();
    m_mapping.x = map[0].toInt();
    m_mapping.y = map[1].toInt();
    m_mapping.cx = map[2].toInt();
    m_mapping.cy = map[3].toInt();
}

void element::read_pos(const QJsonObject &obj)
{
    const auto p = obj[CFG_POS].toArray();
    m_pos.x = p[0].toInt();
    m_pos.y = p[1].toInt();
}
