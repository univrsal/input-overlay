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
    free_handler();
    m_current_handler = new icon_handler();
}

void input_queue::init_text()
{
    free_handler();
    m_current_handler = new text_handler(m_settings);
}

void input_queue::free_handler()
{
    delete m_current_handler;
    m_current_handler = nullptr;
}


input_queue::input_queue(sources::history_settings* settings)
    : m_settings(settings)
{
    m_queued_entry = new input_entry();

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
    free_handler();
    delete m_queued_entry;
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

obs_source_t* input_queue::get_fade_in() const
{
    return nullptr;
}

void input_queue::collect_input() const
{
    m_queued_entry->collect_inputs(m_settings);
}

void input_queue::swap() const
{
    m_current_handler->swap(m_queued_entry);
}

void input_queue::tick(const float seconds) const
{
    m_current_handler->tick(seconds);
}

void input_queue::render(gs_effect_t* effect) const
{
    UNUSED_PARAMETER(effect);
    m_current_handler->render();
}

void input_queue::clear()
{
    
    m_height = 0;
    m_width = 0;
}
