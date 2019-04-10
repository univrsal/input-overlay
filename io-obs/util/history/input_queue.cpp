/**
 * This file is part of input-overlay
 * which is licenced under the
 * MOZILLA PUBLIC LICENCE 2.0
 * See LICENSE or http://www.gnu.org/licenses
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


input_queue::input_queue(sources::history_settings* settings) : m_settings(settings)
{
    m_queued_entry = new input_entry();
}

input_queue::~input_queue()
{
    free_handler();
    delete m_queued_entry;
}

void input_queue::update(const sources::history_mode new_mode)
{
    if (new_mode != m_settings->mode || !m_current_handler) {
        switch (m_settings->mode) {
            default:
            case sources::MODE_TEXT:
                init_text();
                break;
            case sources::MODE_ICONS:
                init_icon();
        }
    }

    m_current_handler->update();
}

obs_source_t* input_queue::get_fade_in() const
{
    const auto h = dynamic_cast<text_handler*>(m_current_handler);
    return h ? h->get_text_source() : nullptr;
}

void input_queue::collect_input() const
{
    m_queued_entry->collect_inputs(m_settings);
}

void input_queue::swap() const
{
    if (!m_queued_entry->empty()) {
        m_current_handler->swap(m_queued_entry);
        m_queued_entry->clear();
    }
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
    /* Not zero, since then the source is hard to 
     * move and click on
     */
    if (m_current_handler)
        m_current_handler->clear();
    m_height = 50;
    m_width = 50;
}
