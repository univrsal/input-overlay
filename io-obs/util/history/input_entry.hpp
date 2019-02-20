/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include "obs.hpp"

class key_names;

namespace sources {
    struct history_settings;
}

class effect;
class element_data_holder;

class input_entry
{
    /* Contains modifiers (Ctrl, Shift, Alt etc)*/
    uint16_t m_mask;
    /* Contains all collected inputs in order */
    std::vector<uint16_t> m_inputs;
    /* Contains all currently active effects */
    std::vector<std::unique_ptr<effect>> m_effects;
    /* Contains input sequence as string*/
    std::string m_text;

    static void mask_to_string(std::string& str, uint16_t mask, bool use_fallback, key_names* names);
public:
    input_entry() = default;
    ~input_entry() = default;

    void collect_inputs(element_data_holder* data);
    void build_string(key_names* names, sources::history_settings* settings);
    
    void tick(float seconds);
    void add_effect(effect* e);

    void render_text(obs_source_t* text_source, sources::history_settings* settings);
    void render_icons(sources::history_settings* settings);
};
