/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <universailp@web.de>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#pragma once

#include "history_icons.hpp"
#include <cstdint>
#include <memory>
#include <obs.hpp>
#include <vector>

class key_names;

namespace sources {
struct history_settings;
}

class effect;

class element_data_holder;

class input_entry {
    /* Contains all collected inputs in order */
    std::vector<uint16_t> m_inputs;
    /* Contains all currently active effects */
    std::vector<std::unique_ptr<effect>> m_effects;

    vec2 m_position {};
    /* Only used in text mode */
    uint16_t m_height = 0, m_width = 0;

    bool m_remove = false; /* Set to true once this entry is the last in the list */
    obs_source_t* m_text_source = nullptr; /* Only used in text mode */
    std::string m_text = "";

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

    void set_text(const char* text, sources::history_settings* settings);

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
