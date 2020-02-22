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

#include "history_icons.hpp"
#include "../../../ccl/ccl.hpp"
#include "input_entry.hpp"
#include <obs-module.h>

void history_icons::unload_texture()
{
    if (m_icon_texture) {
        obs_enter_graphics();
        gs_image_file_free(m_icon_texture);
        m_icon_texture = nullptr;
        obs_leave_graphics();
    }
    m_loaded = false;
}

history_icons::~history_icons()
{
    unload_texture();
}

uint16_t util_read_hex(std::string& l)
{
    std::string temp = l.substr(0, l.find(','));
    l = l.substr(l.find(',') + 1, l.size());

    if (temp.find("0x") != std::string::npos) {
        return std::stoul(temp, nullptr, 16);
    } else {
        return (uint16_t)0x0;
    }
}

void history_icons::load_from_file(const char* cfg, const char* img)
{
    unload_texture();
    if (!cfg || !img)
        return;
    m_icon_texture = new gs_image_file_t();

    gs_image_file_init(m_icon_texture, img);

    obs_enter_graphics();
    gs_image_file_init_texture(m_icon_texture);
    obs_leave_graphics();

    if (!m_icon_texture->loaded) {
        blog(LOG_ERROR, "Error: Failed to load key icon image from %s", img);
        unload_texture();
        return;
    }

    auto cfg_loaded = false;
    auto ccl = ccl_config(cfg, "");

    if (!ccl.is_empty()) {
        cfg_loaded = true;
        m_icon_count = ccl.get_int("icon_count");
        m_icon_w = ccl.get_int("icon_w");
        m_icon_h = ccl.get_int("icon_h");
        const auto node = ccl.get_node("icon_order");

        if (node) {
            auto icon_order = node->get_value();
            for (auto i = 0; i < m_icon_count; i++) {
                icon ico {};
                /* TODO: Alternative? */
                const auto vc = util_read_hex(icon_order);
#ifdef DEBUG
                blog(LOG_DEBUG, "Loaded icon with code 0x%X", vc);
#endif
                ico.u = (m_icon_w + 3) * i + 1;
                ico.v = 1;

                m_icons[vc] = ico;

                if (icon_order.empty()) {
                    m_icon_count = i;
                    break;
                }
            }
        } else {
            blog(LOG_ERROR, "Loading key icons from %s failed. No ccl data!", cfg);
        }
    }

    m_loaded = cfg_loaded && m_icon_texture->loaded;

    if (!cfg_loaded)
        blog(LOG_ERROR, "Error: Failed to load key icon config from %s", cfg);

    if (ccl.has_errors())
        blog(LOG_WARNING, "[input-overlay] %s", ccl.get_error_message().c_str());
}

void history_icons::draw(uint16_t vc, vec2* pos, input_entry* parent)
{
    if (m_icons.find(vc) != m_icons.end()) {
        auto icon = m_icons[vc];
        gs_matrix_push();
        gs_matrix_translate3f(pos->x, pos->y, 0.f);
        parent->render_effects();
        gs_draw_sprite_subregion(m_icon_texture->texture, 0, icon.u, icon.v, m_icon_w + 1, m_icon_h + 1);
        gs_matrix_pop();
    }
}

gs_image_file_t* history_icons::image_file()
{
    return m_icon_texture;
}
