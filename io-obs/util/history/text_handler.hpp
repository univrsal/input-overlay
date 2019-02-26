/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include <string>
#include <vector>
#include <obs.hpp>
#include "key_names.hpp"

class input_entry;

namespace sources {
    struct history_settings;
}

enum text_phase
{
    BLENDING, /* New entry blends in (last entry blends out, if maximum size is exceeded)*/
    DISPLAY   /* all strings in m_values are displayed */
};

class text_handler
{
    std::string m_in;                   /* Fading in text */
    std::vector<std::string> m_values;  /* Text body (All key combinations in order) */
    vec2 m_move_dir;
    /* Since displaying multiple blocks of text isn't easily doable
     * input-history uses three different sources.
     * m_fade_in:  This source draws the currently coming up input combination
     *             and will only be visible during the fade in/out period
     * m_holder:   This source draws all current key combinations
     * m_fade_out: This source draws the last entry that fades out while a new
     *             entry fades in.
     */
    obs_source_t* m_src_body{}, *m_src_in{}, *m_src_out{};

    input_entry* m_entry_in, *m_entry_out, *m_entry_body;
    text_phase m_phase;
    key_names m_names; /* Contains custom key names */
    sources::history_settings* m_settings;

    void make_body_text(std::string& str);
public:
    text_handler(sources::history_settings* settings);
    ~text_handler();

    void load_names(const char* cfg);
    void update();
    void tick(float seconds);
    void swap(input_entry* current);
};
