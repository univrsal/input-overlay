/**
 * Created by univrsal on 05.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "Localization.hpp"
#include "Notifier.hpp"
#include "SDL_helper.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <sys/stat.h>
#else
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include "Localization.hpp"
#endif

Localization::Localization(const char * lang_folder, SDL_helper * helper)
{
	m_lang_folder = lang_folder;
	m_helper = helper;
	scan_lang_folder();

	if (m_lang_files.empty())
	{
		printf("No language files found! GUI will use identifiers instead\n");
	}
	else
	{
		load_default_language();
	}
}

void Localization::load_lang_by_id(uint8_t id)
{
	if (!m_lang_files.empty() && id >= 0 && id < m_lang_files.size())
	{
		
	}
}

std::string Localization::localize(const char * id)
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
		value = m_unlocalized;
	}
	return value;
}

void Localization::scan_lang_folder(void)
{
#ifdef _WIN32
	HANDLE hFind;
	WIN32_FIND_DATA data;
	std::string path = m_lang_folder;
	std::string file_name;
	path.append("/*.ini");
	char def_char = ' ';
	hFind = FindFirstFile(path.c_str(), &data);
	ccl_config * lang = nullptr;

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			file_name =  std::string(data.cFileName);
			if (!(GetFileAttributes(data.cFileName) & FILE_ATTRIBUTE_DIRECTORY));
			{
				lang = new ccl_config(m_lang_folder + "/" + file_name, "");
				ccl_data * node = nullptr;

				if (lang && (node = lang->get_node(LANG_ID)) != nullptr)
				{
					m_lang_files[file_name.erase(file_name.size() - 4)] = node->get_value();
				}
				else
				{
					printf("Invalid lang file %s. Missing language identifier\n", file_name.c_str());
				}
				delete lang;
				lang = nullptr;
			}
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
	else
	{
		printf("Localization: Couldn't load files from lang folder!\n");
	}
#else
	DIR *dir;
	struct dirent *dirent;

	dir = opendir(m_lang_folder.c_str());
	std::string file_name;
	if (dir != NULL)
	{
		while ((dirent = readdir(dir)))
		{
			file_name = std::string(dirent->d_name);
			struct stat path_stat;
			std::string full_path = m_lang_folder + "/" + name;
			stat(full_path.c_str(), &path_stat);

			if (S_ISREG(path_stat.st_mode) == 1)
			{
				m_lang_files.emplace_back(file_name);
			}
		}
	}
	else
	{
		printf("Localization: Couldn't load files from lang folder!\n");
	}

	closedir(dir);
#endif
}

void Localization::load_default_language(void)
{
	if (!m_lang_files["de_DE"].empty())
	{
		std::string path = PATH_TRANSLATIONS +  std::string("/de_DE.ini");
		m_english = std::make_unique<ccl_config>(path, "");
		if (!m_english->is_empty())
		{
			m_valid = true;
		}
		else
		{
			printf("Loading of English translation failed!\n");
		}
	}
	else
	{
		printf("Couldn't load default language! Localization might not work!\n");
	}
}
