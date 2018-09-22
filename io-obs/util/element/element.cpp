/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "element.hpp"
#include "../../ccl/ccl.hpp"

void element::read_mapping(ccl_config* cfg, const std::string& id)
{
    const auto r = cfg->get_rect(id + CFG_MAPPING);
    m_mapping.x = r.x;
    m_mapping.y = r.y;
    m_mapping.cx = r.w;
    m_mapping.cy = r.h;
}

void element::read_pos(ccl_config* cfg, const std::string& id)
{
    const auto p = cfg->get_point(id + CFG_POS);
    m_pos.x = p.x;
    m_pos.y = p.y;
}
