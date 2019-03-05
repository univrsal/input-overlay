/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include <string>
#include <vector>
#include <obs-module.h>
#include <memory>
#include "key_names.hpp"
#include "handler.hpp"

class input_entry;

namespace sources {
    struct history_settings;
}

struct key_combination
{
    key_combination(const std::string str)
    {
        repeat = 1;
        keys = str;
    }

    std::string keys;
    uint8_t repeat;
};

class text_handler
    : public handler
{
    std::vector<std::unique_ptr<key_combination>> m_values;  /* Text body (All key combinations in order) */

    key_names m_names; /* Contains custom key names */
    input_entry* m_display = nullptr;
    sources::history_settings* m_settings = nullptr;
    obs_source_t* m_text_source = nullptr;
    void make_body_text(std::string& str);
public:
    text_handler(sources::history_settings* settings);
    ~text_handler();

    void load_names(const char* cfg);
    void update() override;
    void tick(float seconds) override;
    void swap(input_entry* current) override;
    void render() override;

    obs_source_t* get_text_source() const; /* Used to add the text properties in get_properties_for_history */
};
