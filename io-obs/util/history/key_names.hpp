/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
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

    bool empty() const;

    const char* get_name(uint16_t vc);
};
