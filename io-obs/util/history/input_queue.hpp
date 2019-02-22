/**
 * This file is part of input-overlay
 * which is licenced under the
 * MOZILLA PUBLIC LICENCE 2.0
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include <memory.h>
#include <vector>
#include <memory>
#include "input_entry.hpp"

class input_queue
{
    std::vector<std::unique_ptr<input_entry>> m_entries;
    sources::history_settings* m_settings{};
    input_entry* m_current = nullptr; /* Currently upcoming entry */

    uint16_t m_width = 0, m_height = 0;
public:
    input_queue(sources::history_settings* settings);
    ~input_queue();

    void collect_input() const; /* Accumulates input events in current entry */
    void swap(); /* Adds current entry to the list */
    void tick(float seconds);
    void render(gs_effect_t* effect);
    void clear();
};
