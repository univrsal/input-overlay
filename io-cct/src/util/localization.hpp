/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2019 univrsal <universailp@web.de>.
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

#include <utility>
#include <vector>
#include <string>
#include <memory.h>
#include "font_helper.hpp"
#include "../../../ccl/ccl.hpp"

class ccl_config;

class sdl_helper;

struct LangFile
{
public:
    LangFile(std::string name, std::string lang)
    {
        file_name = std::move(name);
        language = std::move(lang);
    }

    std::string file_name;
    std::string language;
};

class localization
{
public:
    localization(const char* lang_folder, sdl_helper* helper);

    void load_lang_by_id(uint8_t id);

    const std::vector<std::unique_ptr<LangFile>>* get_languages() const
    { return &m_langfiles; }

    std::string localize(const char* id) const;

    uint8_t get_english_id() const
    { return m_english_id; }

private:
    void scan_lang_folder();

    void load_default_language();

    bool m_valid = false;

    uint8_t m_english_id = 0;

    std::string m_lang_folder;
    std::vector<std::unique_ptr<LangFile>> m_langfiles;

    sdl_helper* m_helper = nullptr;

    std::unique_ptr<ccl_config> m_english;
    std::unique_ptr<ccl_config> m_current;
};
