/**
 * Created by univrsal on 05.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "Localization.hpp"
#include "Notifier.hpp"
#include "SDL_Helper.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <sys/stat.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include "Localization.hpp"
#endif

Localization::Localization(const char* lang_folder, SDL_Helper* helper)
{
    m_lang_folder = lang_folder;
    m_helper = helper;
    scan_lang_folder();

    if (m_langfiles.empty() || !m_valid)
    {
        printf("No valid language files found! GUI will use identifiers instead\n");
    }
    else
    {
        load_default_language();
    }
}

void Localization::load_lang_by_id(uint8_t id)
{
    if (m_valid && id >= 0 && id < m_langfiles.size())
    {
        if (id == m_english_id)
        {
            m_current.reset();
            return; /* English is always loaded */
        }

        const auto lang = m_langfiles[id].get();
        if (lang)
        {
            m_current.reset();
            auto path = PATH_TRANSLATIONS + std::string("/") + lang->file_name;
            m_current = std::make_unique<ccl_config>(path, "");
        }
        else
        {
            printf("Can't load language with id %i\n", id);
        }
    }
}

std::string Localization::localize(const char* id) const
{
    std::string value = "";
    if (m_valid)
    {
        if (m_current)
        {
            value = m_current->get_string(id);
            if (value.empty())
                value = m_english->get_string(id);
        }
        else if (m_english)
        {
            value = m_english->get_string(id);
        }
    }

    if (!m_valid || value.empty())
    {
        printf("Couldn't find localization for %s.\n", id);
        value = std::string(id);
    }
    return value;
}

void Localization::scan_lang_folder()
{
    ccl_config* lang = nullptr;
    std::string file_name;
#ifdef _WIN32
    WIN32_FIND_DATA data;
    auto path = m_lang_folder;

    path.append("/*.ini");
    const auto h_find = FindFirstFile(path.c_str(), &data);

    if (h_find != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(GetFileAttributes(data.cFileName) & FILE_ATTRIBUTE_DIRECTORY));
            {
                file_name = std::string(data.cFileName);

#else
    /* Iterating over items in folder on linux
     * and filtering only *.ini files
     */
    DIR * dir;
    struct dirent * dirent;

    dir = opendir(m_lang_folder.c_str());

    if (dir)
    {
        while ((dirent = readdir(dir)))
        {
            file_name = std::string(dirent->d_name);
            struct stat path_stat;
            std::string full_path = m_lang_folder + "/" + file_name;
            stat(full_path.c_str(), &path_stat);
            char * file_type = strrchr(dirent->d_name, '.');
    
            if (file_type && !strcmp(file_type, ".ini") /* Checks file ending */
                    && S_ISREG(path_stat.st_mode))
            {
#endif
                /* After filtering on windows and linux store file name 
                 * in file_name
                 */
                lang = new ccl_config(m_lang_folder + "/" + file_name, "");
                ccl_data* node = nullptr;

                if (lang && (node = lang->get_node(LANG_ID)))
                {
                    m_valid = true;
                    if (file_name == "en_US.ini")
                        m_english_id = m_langfiles.size();
                    m_langfiles.emplace_back(new LangFile(file_name, node->get_value()));
                }
                else
                {
                    printf("Invalid lang file %s. Missing language identifier\n", file_name.c_str());
                }

                delete lang;
                lang = nullptr;
            }
#ifdef _WIN32
        }
        while (FindNextFile(h_find, &data));
        FindClose(h_find);
#else
        }
        closedir(dir);
#endif
    }
    else
    {
        printf("Localization: Couldn't load files from lang folder!\n");
    }
}

void Localization::load_default_language()
{
    auto flag = true;

    if (m_valid)
    {
        auto path = PATH_TRANSLATIONS + std::string("/en_US.ini");
        m_english = std::make_unique<ccl_config>(path, "");
        if (!m_english->is_empty())
        {
            m_valid = true;
        }
        else
        {
            flag = false;
        }
    }

    if (!flag)
    {
        printf("Loading of English translation failed!\n");
        printf("Couldn't load default language! Localization might not work!\n");
    }
}
