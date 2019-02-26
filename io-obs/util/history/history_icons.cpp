/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include <obs-module.h>
#include "history_icons.hpp"
#include "../../../ccl/ccl.hpp"

void history_icons::unload_texture()
{
    if (m_icon_texture)
    {
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

    if (!m_icon_texture->loaded)
    {
        blog(LOG_ERROR, "Error: Failed to load key icon image from %s", img);
        unload_texture();
        return;
    }

    auto cfg_loaded = false;
    auto ccl = ccl_config(cfg, "");

    if (!ccl.is_empty())
    {
        cfg_loaded = true;
        m_icon_count = ccl.get_int("icon_count");
        m_icon_w = ccl.get_int("icon_w");
        m_icon_h = ccl.get_int("icon_h");
        const auto node = ccl.get_node("icon_order");

        if (node)
        {
            auto icon_order = node->get_value();
            for (auto i = 0; i < m_icon_count; i++)
            {
                icon ico{};
                /* TODO: Alternative? */
                const auto vc = 0x00f;// util_read_hex(icon_order);
                ico.u = (m_icon_w + 3) * i + 1;
                ico.v = 1;

                m_icons[vc] = ico;

                if (icon_order.empty())
                {
                    m_icon_count = i;
                    break;
                }
            }
        }
        else
        {
            blog(LOG_ERROR, "Loading key icons from %s failed. No ccl data!",
                cfg);
        }
    }

    m_loaded = cfg_loaded && m_icon_texture->loaded;

    if (!cfg_loaded)
        blog(LOG_ERROR, "Error: Failed to load key icon config from %s", cfg);

    if (ccl.has_errors())
        blog(LOG_WARNING, "[input-overlay] %s", ccl.get_error_message().c_str());
}

void history_icons::draw(uint16_t vc, vec2* pos)
{

}
