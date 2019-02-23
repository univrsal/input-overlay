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
    : m_settings(settings)
{
    m_current = new input_entry();
}

input_queue::~input_queue()
{
    delete m_current;
}

void input_queue::collect_input() const
{
    m_current->collect_inputs(m_settings);
}

void input_queue::swap()
{
    /* Set correct position based on flow direction */
    switch(m_settings->dir)
    {
    case DIR_UP:
        m_current->set_pos(0,
            m_height * (m_settings->history_size + m_settings->v_space));
        break;
    case DIR_LEFT:
        m_current->set_pos(m_width * (m_settings->history_size
            + m_settings->h_space), 0);
        break;
    default:    /* Default is (0,0) top right */
    case DIR_RIGHT:
    case DIR_DOWN:;
    }

    m_entries.emplace_back(m_current);
    /* new entry fill fade in by being scaled up from 0 to 100% */
    m_current->add_effect(new scale_effect(.5f, 1.f));

    /* Now create a new entry which accumulates inputs until next swap() call */
    m_current = new input_entry();
    
    vec2 dir = {};

    switch(m_settings->dir)
    {
        default:
        case DIR_UP: 
            dir = { 0.f, -1.f };
            break;
        case DIR_DOWN:
            dir = { 0.f, 1.f };
            break;
        case DIR_LEFT:
            dir = { -1.f, 0.f };
            break;
        case DIR_RIGHT:
            dir = { 1.f, 0.f };
            break;
    }

    /* Add the translation effect, which smoothly moves all entries back to make
     * space for the new entry
     */
    for (auto& entry : m_entries)
    {
        /* The icon mode draws each icon at the correct position, while the text
         * mode draws the text source by translating the matrix in the correct position
         * so any mode that uses text has to set translation to true
         */
        entry->add_effect(new translate_effect(.5f, dir, entry->get_pos(),
            m_settings->mode != sources::MODE_ICONS));
    }

    /* Mark the last entry for removal, meaning that after the fade out effect is over
     * it'll be deleted
     */
    m_entries.front()->mark_for_removal();
    /* Add fading out effect */
    m_entries.front()->add_effect(new scale_effect(.5f, -1.f, 1.f));
}

void input_queue::tick(float seconds)
{
    const auto calc_size = m_width == 0 || m_height == 0;

    for (auto& entry : m_entries)
    {
        entry->tick(seconds);

        if (calc_size)
        {
            switch (m_settings->dir)
            {
                /* if moving up/down the lines expand horizontal
                 * which means that the width of the source is
                 * the longest line
                 */
            default:
            case DIR_DOWN:
            case DIR_UP:
                if (entry->get_width() > m_width)
                    m_width = entry->get_width();
                break;
                /* if moving left/right the lines expand vertical
                 * which means that the height of the source is
                 * the highest line
                 */
            case DIR_LEFT:
            case DIR_RIGHT:
                if (entry->get_width() > m_width)
                    m_height = entry->get_height();
            }
        }
    }

    if (calc_size)
    {
        if (m_entries.empty())
        {
            m_width = 50;
            m_height = 50;
        }
        else
        {
            switch (m_settings->dir)
            {
            default: /* Width/height respectively have been calculated above */
            case DIR_DOWN:
            case DIR_UP:
                /* height is constant */
                m_height = m_entries[0]->get_height();
                break;
            case DIR_LEFT:
            case DIR_RIGHT:
                /* width is constant */
                m_width = m_entries[0]->get_width();
            }
        }
    }
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
            entry->render_icons(m_settings);
    }
}

void input_queue::clear()
{
    m_entries.clear();
    m_height = 0;
    m_width = 0;
}
