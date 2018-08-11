/**
 * Created by univrsal on 05.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include <vector>
#include <string>
#include <memory.h>
#include "../../../ccl/ccl.hpp"

class ccl_config;

class SDL_helper;

struct LangFile
{
public:
	LangFile(std::string name, std::string lang)
	{
		file_name = name;
		language = lang;
	}
	std::string file_name;
	std::string language;
};

class Localization
{
public:
	Localization(const char * lang_folder, SDL_helper * h);

	void load_lang_by_id(uint8_t id);
	const std::vector<std::unique_ptr<LangFile>> * get_languages(void) { return &m_langfiles; }

	std::string localize(const char * id);
	uint8_t get_english_id(void) { return m_english_id; }
private:
	void scan_lang_folder(void);
	void load_default_language(void);

	bool m_valid = false;

	uint8_t m_english_id = 0;

	std::string m_lang_folder;
	std::vector<std::unique_ptr<LangFile>> m_langfiles;

	SDL_helper * m_helper = nullptr;

	std::unique_ptr<ccl_config> m_english;
	std::unique_ptr<ccl_config> m_current;
};
