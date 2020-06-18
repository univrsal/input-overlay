/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <universailp@web.de>.
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

#include "font_helper.hpp"
#include "util.hpp"
#include <memory.h>
#include <string>
#include <utility>
#include <vector>

class sdl_helper;

class lang_file {
public:
	lang_file(const std::string &name, const std::string &lang)
	{
		m_file_path = name;
		m_language = lang;
	}

	std::string m_file_path;
	std::string m_language;
};

class localization {
public:
	localization(const char *lang_folder, sdl_helper *helper);

	void load_lang_by_id(uint8_t id);

	const std::vector<lang_file> *get_languages() const { return &m_langfiles; }

	std::string localize(const char *id) const;

	uint8_t get_english_id() const { return m_english_id; }

private:
	void scan_lang_folder();

	void load_default_language();

	bool m_have_default = false;

	uint8_t m_english_id = 0;

	std::string m_lang_folder;
	std::vector<lang_file> m_langfiles;

	sdl_helper *m_helper = nullptr;

	json11::Json m_english;
	json11::Json m_current;
};
