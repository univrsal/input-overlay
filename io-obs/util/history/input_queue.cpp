/**
 * This file is part of input-overlay
 * which is licenced under the
 * MOZILLA PUBLIC LICENCE 2.0
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "input_queue.hpp"
#include "sources/input_history.hpp"
#include "icon_handler.hpp"
#include "text_handler.hpp"

void input_queue::init_icon()
{
    free_text();
    m_icon_handler = new icon_handler();
}

void input_queue::init_text()
{
    free_icon();
    m_text_handler = new text_handler(m_settings);
}

void input_queue::free_icon()
{
    delete m_icon_handler;
    m_icon_handler = nullptr;
}

void input_queue::free_text()
{
    delete m_text_handler;
    m_text_handler = nullptr;
}

input_queue::input_queue(sources::history_settings* settings)
    : m_settings(settings)
{
    m_current = new input_entry();

    switch(settings->mode)
    {
    default:
    case sources::MODE_COMMANDS:
    case sources::MODE_TEXT:
        init_text();
        break;
    case sources::MODE_ICONS:
        init_icon();
    }
}

input_queue::~input_queue()
{
    free_text();
    free_icon();
    delete m_current;
}

void input_queue::update()
{
    switch (m_settings->mode)
    {
    default:
    case sources::MODE_COMMANDS:
    case sources::MODE_TEXT:
        init_text();
        break;
    case sources::MODE_ICONS:
        init_icon();
    }
}

obs_source_t* input_queue::get_fade_in()
{
    return nullptr;
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

    }
    else
    {

    }
}

void input_queue::clear()
{
    m_entries.clear();
    m_height = 0;
    m_width = 0;
}
