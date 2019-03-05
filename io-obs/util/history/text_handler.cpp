/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "text_handler.hpp"
#include "sources/input_history.hpp"
#include "input_entry.hpp"
#include "key_names.hpp"


#ifdef _WIN32
#define TEXT_SOURCE "text_gdiplus\0"
#else
#define TEXT_SOURCE "text_ft2_source\0"
#endif

void text_handler::make_body_text(std::string& str)
{
    switch(m_settings->dir)
    {
    case DIR_DOWN:
    case DIR_LEFT:
        for (const auto& line : m_values)
        {
            str += line->keys;
            if (line->repeat > 1)
                str += " (" + std::to_string(line->repeat) + ")\n";
            else
                str += "\n";
        }
        break;
    default:;
        for (auto line = m_values.rbegin(); line != m_values.rend(); ++line)
        {
            str += line->get()->keys;
            if (line->get()->repeat > 1)
                str += " (" + std::to_string(line->get()->repeat) + ")\n";
            else
                str += "\n";
        }
    }
 
    if (str.length() > 2)
    {
        str.pop_back(); str.pop_back(); /* Get rid of last '\n' */
    }
}

text_handler::text_handler(sources::history_settings* settings)
    : m_settings(settings)
{

    /* The body source uses the input-history settings */
    m_text_source = obs_source_create(TEXT_SOURCE, "history-fade-out-text",
        settings->settings, nullptr);
    obs_source_add_active_child(settings->source, m_text_source);
    m_display = new input_entry(m_text_source);
}

text_handler::~text_handler()
{

    obs_source_remove(m_text_source);
    obs_source_release(m_text_source);
    m_text_source = nullptr;
    delete m_display;
    m_display = nullptr;
}

void text_handler::load_names(const char* cfg)
{
    m_names.load_from_file(cfg);
}

void text_handler::update()
{
    switch(m_settings->dir)
    {
    case DIR_LEFT:
    case DIR_RIGHT:
        obs_data_set_bool(m_settings->settings, "vertical", true);
        break;
    default:
        obs_data_set_bool(m_settings->settings, "vertical", false);
    }

    obs_source_update(m_text_source, m_settings->settings);
}

void text_handler::tick(const float seconds)
{
}

void text_handler::swap(input_entry* current)
{
    auto new_line = current->build_string(&m_names,
        m_settings->flags & sources::FLAG_USE_FALLBACK);

    if (!m_values.empty() && m_values.begin()->get()->keys == new_line)
    {
        if (m_settings->flags & sources::FLAG_REPEAT_KEYS)
            m_values.begin()->get()->repeat++;
    }
    else
    {
        m_values.insert(m_values.begin(), std::make_unique<key_combination>(new_line));
    }


    /* If the currently displayed exceed the history size */
    if (m_values.size() > m_settings->history_size)
        m_values.pop_back();

    std::string text;
    make_body_text(text);
    m_display->set_text(text.c_str(), m_settings->settings);
}

void text_handler::render()
{
    m_display->render_text();
}

obs_source_t* text_handler::get_text_source() const
{
    return m_text_source;
}
