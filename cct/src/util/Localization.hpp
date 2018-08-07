/**
 * Created by univrsal on 05.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include <map>
#include <string>
#include <memory.h>
#include "../../../ccl/ccl.hpp"

class ccl_config;

class SDL_helper;

class Localization
{
public:
	Localization(const char * lang_folder, SDL_helper * h);

	void load_lang_by_id(uint8_t id);
	const std::map<std::string, std::string> * get_languages(void) { return &m_lang_files; }

	std::string localize(const char * id);
private:
	void scan_lang_folder(void);
	void load_default_language(void);

	bool m_valid = false;

	std::string m_lang_folder;
	std::string m_unlocalized = "<unlocalized>";
	std::map<std::string, std::string> m_lang_files;

	SDL_helper * m_helper = nullptr;

	std::unique_ptr<ccl_config> m_english;
	std::unique_ptr<ccl_config> m_current;
};
