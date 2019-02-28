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
#include "scale_effect.hpp"
#include "translate_effect.hpp"

#ifdef _WIN32
#define TEXT_SOURCE "text_gdiplus\0"
#else
#define TEXT_SOURCE "text_ft2_source\0"
#endif

/* The text mode works as follows:
 * There's two different phases
 *  - One for blending in/out
 *    (this is used once a new key combination was pressed and swap() is called)
 *  - One for plain displaying
 *    (This is during the time new inputs are collected)
 *    
 * During display all strings in m_values are displayed over the m_holder
 * obs text source
 * Once swap() is called the new combination will be converted into a string
 * and displayed over m_fade_in, while the last string in m_values will,
 * if the amount of key combinations in m_value exceeds the configured size of
 * input-history, be removed and put into m_fade_out.
 * 
 * After the fading cycle is over m_fade_out and m_fade_in will no longer be displayed
 * and the text in m_fade_in will be added to m_values
 */

void text_handler::make_body_text(std::string& str)
{
    for (const auto& line : m_values)
        str += line + "\n";
    if (str.length() > 2)
    {
        str.pop_back(); str.pop_back(); /* Get rid of last '\n' */
    }
}

text_handler::text_handler(sources::history_settings* settings)
    : m_settings(settings)
{

    /* The body source uses the input-history settings */
    m_src_body = obs_source_create(TEXT_SOURCE, "history-body-text",
        settings->settings, nullptr);

    m_src_in = obs_source_create(TEXT_SOURCE, "history-fade-in-text",
        nullptr, nullptr);
    m_src_out = obs_source_create(TEXT_SOURCE, "history-fade-out-text",
        nullptr, nullptr);
    
    obs_source_add_active_child(settings->source, m_src_in);
    obs_source_add_active_child(settings->source, m_src_out);
    obs_source_add_active_child(settings->source, m_src_body);

    m_entry_in = new input_entry(m_src_in);
    m_entry_out = new input_entry(m_src_out);
    m_entry_body = new input_entry(m_src_body);
}

text_handler::~text_handler()
{

    obs_source_remove(m_src_in);
    obs_source_remove(m_src_out);
    obs_source_remove(m_src_body);

    obs_source_release(m_src_in);
    obs_source_release(m_src_out);
    obs_source_release(m_src_body);

    m_src_in = nullptr;
    m_src_out = nullptr;
    m_src_body = nullptr;
}

void text_handler::load_names(const char* cfg)
{
    m_names.load_from_file(cfg);
}

void text_handler::update()
{
    switch (m_settings->dir)
    {
    case DIR_DOWN:
        m_move_dir = { 0.f, 1.f };
        /* the height of m_entry_in (or entry_out) is the height of one line
         * using the current font settings
         */
        m_entry_in->set_pos(0, 0);
        m_entry_body->set_pos(0, m_entry_in->get_height() + m_settings->v_space);
        m_entry_out->set_pos(0, (m_entry_in->get_height() + m_settings->v_space)
            * (m_settings->history_size + 1));
        /* Size + 1 since the layout is as follows (if direction is down) :
         * [ entry_in        ] <-- + 1           +- Flow direction
         * [ History entry 1 ] <-+ history_size  |
         * [ ...             ]   |               |
         * [ History entry n ] <-+               |
         * [ entry_out       ]                   V
         */
        break;
    case DIR_UP: 
        m_move_dir = { 0.f, -1.f };
        m_entry_out->set_pos(0, 0);
        m_entry_body->set_pos(0, m_entry_in->get_height() + m_settings->v_space);
        m_entry_in->set_pos(0, (m_entry_in->get_height() + m_settings->v_space)
            * (m_settings->history_size + 1));
        break;
    case DIR_LEFT:
        m_move_dir = { -1.f,0.f };
        m_entry_out->set_pos(0, 0);
        m_entry_body->set_pos(m_entry_in->get_width() + m_settings->h_space, 0);
        m_entry_in->set_pos((m_entry_in->get_width() + m_settings->h_space)
            * (m_settings->history_size + 1), 0);
        break;
    case DIR_RIGHT:
        m_move_dir = { 1.f, 0.f };
        m_entry_in->set_pos(0, 0);
        m_entry_body->set_pos(m_entry_in->get_width() + m_settings->h_space, 0);
        m_entry_out->set_pos((m_entry_in->get_width() + m_settings->h_space)
            * (m_settings->history_size + 1), 0);
        break;
    default: ;
    }

    obs_source_update(m_src_body, m_settings->settings);
    obs_source_update(m_src_in, m_settings->settings);
    obs_source_update(m_src_out, m_settings->settings);
}

void text_handler::tick(const float seconds)
{
    if (m_phase == BLENDING)
    {
        m_entry_in->tick(seconds);
        m_entry_out->tick(seconds);
        m_entry_body->tick(seconds);

        if (m_entry_in->effects_finished() || m_entry_out->effects_finished()
            || m_entry_body->effects_finished())
        {
            m_phase = DISPLAY;
            m_values.insert(m_values.begin(), m_in);
            std::string new_text;
            make_body_text(new_text);
            m_entry_body->set_text(new_text.c_str(), m_settings->settings);
            update(); /* Reset position */
        }
    }
}

void text_handler::swap(input_entry* current)
{
    m_phase = BLENDING;

    /* Gets the current key combination as a string and sets it
     * as the text for the fade_in text source
     */
    m_in = current->build_string(&m_names,
        m_settings->flags & sources::FLAG_USE_FALLBACK);
    m_entry_in->set_text(m_in.c_str(), m_settings->settings);
    blog(LOG_DEBUG, "Swap: %s", m_in.c_str());

    /* If the currently displayed exceed the history size (the currently upcoming value
     * also counts) take the last entry and fade it out
     */
    if (m_values.size() >= m_settings->history_size)
    {
        m_entry_out->set_text(m_values.back().c_str(), m_settings->settings);
        /* The last line is displayed over m_entry_out so it can be removed here */
        m_values.pop_back();
        std::string new_text;
        make_body_text(new_text);
        m_entry_body->set_text(new_text.c_str(), m_settings->settings);
    }

    /* Add the fading in/out effect */
    m_entry_in->add_effect(new scale_effect(0.5f, 1.f, 0.f));
    m_entry_out->add_effect(new scale_effect(0.5f, -1.f, 1.0f));

    /* Now based on the flow direction of input-history the sources 
     * have to be positioned correctly*/
    m_entry_in->add_effect(new translate_effect(0.5, m_move_dir, m_entry_in->get_pos(),
        true));
    m_entry_out->add_effect(new translate_effect(0.5, m_move_dir, m_entry_out->get_pos(),
        true));
    m_entry_body->add_effect(new translate_effect(0.5, m_move_dir, m_entry_in->get_pos(),
        true));

    update();
}

void text_handler::render()
{
    m_entry_in->render_text();
    m_entry_body->render_text();
    m_entry_out->render_text();
}

obs_source_t* text_handler::get_src_in() const
{
    return m_src_in;
}
