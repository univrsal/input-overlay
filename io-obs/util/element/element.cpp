/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "element.hpp"
#include "../../ccl/ccl.hpp"

void element::read_size(ccl_config* cfg, const std::string& id,
                        const vec2* default_dim)
{
    if (cfg->node_exists(id + CFG_WIDTH, true))
        m_mapping.cx = cfg->get_int(id + CFG_WIDTH) + CFG_OUTER_BORDER * 2;
    else
        m_mapping.cx = default_dim->x;

    if (cfg->node_exists(id + CFG_HEIGHT, true))
        m_mapping.cy = cfg->get_int(id + CFG_HEIGHT) + CFG_OUTER_BORDER * 2;
    else
        m_mapping.cy = default_dim->y;
}

void element::read_pos(ccl_config* cfg, const std::string& id)
{
    m_pos.x = cfg->get_int(id + CFG_X_POS) - CFG_OUTER_BORDER;
    m_pos.y = cfg->get_int(id + CFG_Y_POS) - CFG_OUTER_BORDER;
}

void element::read_uv(ccl_config* cfg, const std::string& id)
{
    m_mapping.x = cfg->get_int(id + CFG_U) - CFG_OUTER_BORDER;
    m_mapping.y = cfg->get_int(id + CFG_V) - CFG_OUTER_BORDER;
}
