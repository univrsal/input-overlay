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

class icon_handler;
class text_handler;

class input_queue
{
    std::vector<std::unique_ptr<input_entry>> m_entries;
    sources::history_settings* m_settings{};
    uint16_t m_width = 0, m_height = 0;

    input_entry* m_current = nullptr;
    text_handler* m_text_handler = nullptr;
    icon_handler* m_icon_handler = nullptr;

    /* Prepare/free the respective display modes */
    void init_icon();
    void init_text();

    void free_icon();
    void free_text();
public:
    input_queue(sources::history_settings* settings);
    ~input_queue();
    /* Any of the three sources could be used, since this is only used to get the 
     * text source properties */
    obs_source_t* get_fade_in();

    void collect_input() const; /* Accumulates input events in current entry */
    void swap(); /* Adds current entry to the list */
    void tick(float seconds);
    void update();
    void render(gs_effect_t* effect);
    void clear();
};
