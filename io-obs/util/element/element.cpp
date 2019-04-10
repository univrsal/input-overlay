/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

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
    m_type = INVALID;
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
    return NONE;
}

void element::read_mapping(ccl_config* cfg, const std::string &id)
{
    const auto r = cfg->get_rect(id + CFG_MAPPING);
    m_mapping.x = r.x;
    m_mapping.y = r.y;
    m_mapping.cx = r.w;
    m_mapping.cy = r.h;
}

void element::read_pos(ccl_config* cfg, const std::string &id)
{
    const auto p = cfg->get_point(id + CFG_POS);
    m_pos.x = p.x;
    m_pos.y = p.y;
}
