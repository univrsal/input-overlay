/**
 * Created by univrsal on 05.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <utility>
#include <vector>
#include <string>
#include <memory.h>
#include "FontHelper.hpp"
#include "../../../ccl/ccl.hpp"

class ccl_config;

class SDL_Helper;

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

class Localization
{
public:
    Localization(const char* lang_folder, SDL_Helper* helper);

    void load_lang_by_id(uint8_t id);
    const std::vector<std::unique_ptr<LangFile>>* get_languages() const { return &m_langfiles; }

    std::string localize(const char* id) const;
    uint8_t get_english_id() const { return m_english_id; }

    bool is_roman() const; /* True if selected language uses roman alphabet */

    /* Return required font for current language */
    uint8_t get_font() const { return is_roman() ? FONT_ROBOTO_SMALL : FONT_WSTRING; }
private:
    void scan_lang_folder();
    void load_default_language();

    bool m_valid = false;
    bool m_roman = true;

    uint8_t m_english_id = 0;

    std::string m_lang_folder;
    std::vector<std::unique_ptr<LangFile>> m_langfiles;

    SDL_Helper* m_helper = nullptr;

    std::unique_ptr<ccl_config> m_english;
    std::unique_ptr<ccl_config> m_current;
};
