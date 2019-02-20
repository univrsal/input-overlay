/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include <string>
#include <map>

class key_names
{
    std::map<uint16_t, std::string> m_names;

public:
    key_names() = default;
    ~key_names() = default;

    void load_from_file(const char* path);

    const char* get_name(const uint16_t vc);
};
