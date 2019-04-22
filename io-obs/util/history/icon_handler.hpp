/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "history_icons.hpp"
#include "handler.hpp"
#include "input_entry.hpp"
#include <deque>
#include <memory>

enum icon_state {
    STATE_BLENDING, /* New entry is blending in, last entry blending out */
    STATE_DISPLAY   /* Only current entries are being rendered */
};

class icon_handler : public handler
{
    history_icons m_icons;
    std::deque<std::unique_ptr<input_entry>> m_entries;
    icon_state m_state = STATE_DISPLAY;
    vec2 m_translate_dir = { 0.f, 1.f }; /* Direction the entries move */
    vec2 m_start_pos = { 0.f, 0.f };     /* Position, where new entries start */
    int m_old_icon_count = 0;            /* Used to calculate and update source size */
public:
    icon_handler(sources::history_settings* settings);

    ~icon_handler() override;

    void load_icons(const char* img, const char* cfg);

    void update() override;

    void tick(float seconds) override;

    void swap(input_entry &current) override;

    void render(const gs_effect_t* effect) override;

    void clear() override;
};
