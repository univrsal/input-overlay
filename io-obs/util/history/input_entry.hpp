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

    vec2 m_position{};
    uint16_t m_height = 0, m_width = 0;

    /* Converts input mask (additional info for input event like holding Ctrl or shift) to string*/
    static void mask_to_string(std::string& str, uint16_t mask, bool use_fallback, key_names* names);

    bool m_remove = false; /* Set to true once this entry is the last in the list */
public:
    input_entry(vec2 pos);
    ~input_entry() = default;

    uint16_t get_width() const;
    uint16_t get_height() const;

    void collect_inputs(sources::history_settings* settings);
    void build_string(sources::history_settings* settings);
    void tick(float seconds);
    void add_effect(effect* e);
    void render_text(sources::history_settings* settings);
    void render_icons(sources::history_settings* settings);

    void mark_for_removal();
    bool finished() const;
};
