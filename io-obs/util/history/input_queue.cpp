/**
 * This file is part of input-overlay
 * which is licenced under the
 * MOZILLA PUBLIC LICENCE 2.0
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include  "input_queue.hpp"
#include "scale_effect.hpp"
#include "translate_effect.hpp"
#include "sources/input_history.hpp"

input_queue::input_queue(sources::history_settings* settings)
    : m_settings(settings), m_holder(nullptr), m_fade_in(nullptr), m_fade_out(nullptr)
{
    m_current = new input_entry();
}

input_queue::~input_queue()
{
    delete m_current;
    m_entries.clear();
    free_text();
}

#ifdef _WIN32
#define TEXT_SOURCE "text_gdiplus\0"
#else
#define TEXT_SOURCE "text_ft2_source\0"
#endif

void input_queue::init_text()
{
    if (!m_last)
        m_last = new input_entry();

    m_fade_in = obs_source_create(TEXT_SOURCE, "history-text-source",
        m_settings->settings, nullptr);
    m_fade_out = obs_source_create(TEXT_SOURCE, "history-text-source",
        m_settings->settings, nullptr);
    m_holder = obs_source_create(TEXT_SOURCE, "history-text-source",
        m_settings->settings, nullptr);

    obs_source_add_active_child(m_settings->source, m_fade_in);
    obs_source_add_active_child(m_settings->source, m_fade_out);
    obs_source_add_active_child(m_settings->source, m_holder);
}

void input_queue::free_text()
{
    delete m_last;
    m_last = nullptr;

    obs_source_remove(m_fade_in);
    obs_source_remove(m_fade_out);
    obs_source_remove(m_holder);

    obs_source_release(m_fade_in);
    obs_source_release(m_fade_out);
    obs_source_release(m_holder);

    m_fade_in = nullptr;
    m_fade_out = nullptr;
    m_holder = nullptr;
}

void input_queue::update(obs_data_t* settings)
{
    if (m_fade_in)
        init_text();

    obs_source_update(m_fade_in, settings);
    obs_source_update(m_fade_out, settings);
    obs_source_update(m_holder, settings);
}

obs_source_t* input_queue::get_fade_in()
{
    if (!m_fade_in)
        init_text();
    return m_fade_in;
}

void input_queue::collect_input() const
{
    m_current->collect_inputs(m_settings);
}

void input_queue::swap()
{

}

void input_queue::tick(const float seconds)
{

}

void input_queue::render(gs_effect_t* effect)
{
    if (m_settings->mode == sources::MODE_ICONS)
    {
        for (auto& entry : m_entries)
            entry->render_icons(m_settings);
    }
    else
    {
        for (auto& entry : m_entries)
            entry->render_text(m_settings);
    }
}

void input_queue::clear()
{
    m_entries.clear();
    m_height = 0;
    m_width = 0;
}
