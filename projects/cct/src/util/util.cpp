/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
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

#include "util.hpp"
#include "sdl_helper.hpp"
#include <fstream>
#include <locale>
#include <sstream>

SDL_bool util_move_element(int *x, int *y, const SDL_Keycode key)
{
	switch (key) {
	case SDLK_UP:
		*y = UTIL_MAX(*y - 1, 0);
		return SDL_TRUE;
	case SDLK_DOWN:
		(*y)++;
		return SDL_TRUE;
	case SDLK_RIGHT:
		(*x)++;
		return SDL_TRUE;
	case SDLK_LEFT:
		*x = UTIL_MAX(0, *x - 1);
		return SDL_TRUE;
	default:;
	}
	return SDL_FALSE;
}

void util::replace(std::string &str, const char *find, const char *replace)
{
	size_t start = 0;
	const auto len = strlen(find);

	while ((start = str.find(find)) != std::string::npos) {
		str.replace(start, len, replace);
	}
}

bool util::load_json(const std::string &path, std::string &err, json11::Json &out)
{
	bool result = false;
#if WIN32
	std::ifstream in(sdl_helper::util_utf8_to_wstring(path));
#else
	std::ifstream in(path.c_str());
#endif
	in.imbue(std::locale("en_US.UTF8"));

	if (in.good()) {
		std::string content = std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
		out = json11::Json::parse(content, err);
		if (err.empty() && !out.is_null()) {
			result = true;
		} else {
			printf("Error loading json from %s: %s\n", path.c_str(), err.c_str());
		}
		result = true;
	}
	return result;
}

bool util::is_empty(const std::string &path)
{
#if WIN32
	std::ifstream input(sdl_helper::util_utf8_to_wstring(path));

#else
	std::ifstream input(path.c_str());
#endif

	return input.peek() == std::ifstream::traits_type::eof();
}

bool util::can_access(const std::string &path)
{
#if WIN32
	std::ofstream output(sdl_helper::util_utf8_to_wstring(path));
#else
	std::ofstream output(path.c_str());
#endif
	bool result = output.good();
	output.close();
	return result;
}

void util::indent_json(std::string &json)
{
	std::stringstream tmp;
	int indent_level = 0;
	char prev = 0;

	for (auto it = json.begin(); it < json.end(); it++) {
		switch (*it) {
		case '{':
		case '[':
			indent_level++;
		case ',':
			tmp << *it << '\n';
			for (int i = 0; i < indent_level; i++)
				tmp << "    ";
			break;
		case '}':
		case ']':
			tmp << "\n";
			indent_level--;
			for (int i = 0; i < indent_level; i++)
				tmp << "    ";
			tmp << *it;
			break;
		case '"':
			prev = 0;
			do {
				tmp << *it;
				prev = *it++;
			} while (*it != '"' || prev == '\\');
			tmp << *it;
			break;
		case ' ':
			break;
		case ':':
			tmp << *it << " ";
			break;
		default:
			tmp << *it;
		}
	}
	json = tmp.str();
}
