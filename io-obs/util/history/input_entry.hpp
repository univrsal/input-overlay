/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <obs.hpp>
#include "history_icons.hpp"

class key_names;

namespace sources
{
    struct history_settings;
}

class effect;

class element_data_holder;

class input_entry
{
    /* Contains all collected inputs in order */
    std::vector<uint16_t> m_inputs;
    /* Contains all currently active effects */
    std::vector<std::unique_ptr<effect>> m_effects;

    vec2 m_position{};
    /* Only used in text mode */
    uint16_t m_height = 0, m_width = 0;

    bool m_remove = false; /* Set to true once this entry is the last in the list */
    obs_source_t* m_text_source = nullptr; /* Only used in text mode */
public:
    explicit input_entry(obs_source_t* source);

    input_entry(input_entry& e);

    input_entry();

    ~input_entry();

    uint16_t get_width() const;

    uint16_t get_height() const;

    uint16_t get_input_count() const;

    vec2* get_pos();

    std::string build_string(key_names* names, bool use_fallback);

    void set_pos(float x, float y);

    void set_text(const char* text, obs_data_t* settings);

    void collect_inputs(sources::history_settings* settings);

    void tick(float seconds);

    void add_effect(effect* e);

    void render_text();

    void clear_effects();

    void render_effects();

    void render_icons(sources::history_settings* settings, history_icons* icons);

    void clear();

    void mark_for_removal();

    bool finished() const;

    bool empty() const;

    void test();


};
