/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "key_names.hpp"
#include "handler.hpp"
#include <string>
#include <vector>
#include <obs-module.h>
#include <memory>

class input_entry;

namespace sources
{
    struct history_settings;
}

struct key_combination
{
    explicit key_combination(const std::string str)
    {
        repeat = 1;
        keys = str;
    }

    std::string keys;
    uint8_t repeat;
};

class text_handler : public handler
{
    std::vector<std::unique_ptr<key_combination>> m_values;  /* Text body (All key combinations in order) */

    key_names m_names; /* Contains custom key names */
    input_entry* m_display = nullptr;
    sources::history_settings* m_settings = nullptr;
    obs_source_t* m_text_source = nullptr;

    void make_body_text(std::string &str);

public:
    explicit text_handler(sources::history_settings* settings);

    ~text_handler() override;

    void load_names(const char* cfg);

    void update() override;

    void tick(float seconds) override;

    void swap(input_entry* current) override;

    void render() override;

    void clear() override;

    obs_source_t* get_text_source() const; /* Used to add the text properties in get_properties_for_history */
};
