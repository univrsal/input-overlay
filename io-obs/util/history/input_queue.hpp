/**
 * This file is part of input-overlay
 * which is licenced under the
 * MOZILLA PUBLIC LICENCE 2.0
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include <vector>
#include <memory>
#include "input_entry.hpp"

class input_queue
{
    std::vector<std::unique_ptr<input_entry>> m_entries;
    sources::history_settings* m_settings{};
    input_entry* m_current = nullptr; /* Currently upcoming entry */
    input_entry* m_last = nullptr; /* Fading out entry when using text mode */

    uint16_t m_width = 0, m_height = 0;

    /* Since displaying multiple blocks of text isn't easily doable
     * input-history uses three different sources.
     * m_fade_in:  This source draws the currently coming up input combination
     *             and will only be visible during the fade in/out period
     * m_holder:   This source draws all current key combinations
     * m_fade_out: This source draws the last entry that fades out while a new
     *             entry fades in.
     */
    obs_source_t* m_holder, *m_fade_in, *m_fade_out;

public:
    input_queue(sources::history_settings* settings);
    ~input_queue();

    void init_text();
    void free_text();
    void update(obs_data_t* settings);
    /* Any of the three sources could be used, since this is only used to get the 
     * text source properties */
    obs_source_t* get_fade_in();

    void collect_input() const; /* Accumulates input events in current entry */
    void swap(); /* Adds current entry to the list */
    void tick(float seconds);
    void render(gs_effect_t* effect);
    void clear();
};
