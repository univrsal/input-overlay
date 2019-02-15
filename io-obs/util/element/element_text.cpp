/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "../../sources/input_source.hpp"
#include "../../../ccl/ccl.hpp"
#include "element_text.hpp"
#include "util/layout_constants.hpp"
#include "element_mouse_movement.hpp"

void element_text::load(ccl_config* cfg, const std::string& id)
{
    read_pos(cfg, id);
    m_keycode = VC_MOUSE_DATA;
    m_use_total_values = cfg->get_bool(id + CFG_TEXT_RESET);
    m_unformatted_text = cfg->get_string(id + CFG_TEXT);
}

void element_text::draw(gs_effect_t* effect,
    gs_image_file_t* image, element_data* data, sources::shared_settings* settings)
{
    if (settings->text_source && settings->data)
    {
        if (data && data->get_type() == MOUSE_STATS)
        {
            auto stats = dynamic_cast<element_data_mouse_stats*>(data);
            if (stats)
            {
                auto formatted = m_unformatted_text;
                format(formatted, "%w", m_use_total_values ? stats->get_wheel_total() : stats->get_wheel_current());
                format(formatted, "%l", m_use_total_values ? stats->get_lmb_total() : stats->get_lmb_current());
                format(formatted, "%r", m_use_total_values ? stats->get_rmb_total() : stats->get_rmb_current());
                format(formatted, "%m", m_use_total_values ? stats->get_mmb_total() : stats->get_mmb_current());
                format(formatted, "%x", m_use_total_values ? stats->get_mmb_total() : stats->get_mmb_current());
                format(formatted, "%y", m_use_total_values ? stats->get_mouse_x() : stats->get_mouse_x());

                obs_data_set_string(settings->data, "text", formatted.c_str());
                obs_source_update(settings->text_source, settings->data);
            }
        }
        
        obs_source_video_render(settings->text_source);
    }
}

data_source element_text::get_source()
{
    return DEFAULT;
}

void element_text::format(std::string& str, const char* find, int replace) const
{
    size_t pos;
    while ((pos = str.find(find)) != std::string::npos)
        str.replace(pos, strlen(find), std::to_string(replace));
}

element_text::element_text()
    : element(TEXT)
{
};
