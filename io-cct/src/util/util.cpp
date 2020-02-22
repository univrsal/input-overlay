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

#include "util.hpp"

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
