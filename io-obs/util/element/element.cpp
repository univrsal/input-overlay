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

#include "element.hpp"
#include "../../../ccl/ccl.hpp"
#include "util/layout_constants.hpp"

element_data::element_data(const element_type type)
{
	m_type = type;
}

element_type element_data::get_type() const
{
	return m_type;
}

element::element() : m_keycode(0)
{
	m_type = ET_INVALID;
}

element::element(const element_type type)
{
	m_type = type;
}

element_type element::get_type() const
{
	return m_type;
}

uint16_t element::get_keycode() const
{
	return m_keycode;
}

data_source element::get_source()
{
	return DS_NONE;
}

void element::read_mapping(ccl_config *cfg, const std::string &id)
{
	const auto r = cfg->get_rect(id + CFG_MAPPING);
	m_mapping.x = r.x;
	m_mapping.y = r.y;
	m_mapping.cx = r.w;
	m_mapping.cy = r.h;
}

void element::read_pos(ccl_config *cfg, const std::string &id)
{
	const auto p = cfg->get_point(id + CFG_POS);
	m_pos.x = p.x;
	m_pos.y = p.y;
}
