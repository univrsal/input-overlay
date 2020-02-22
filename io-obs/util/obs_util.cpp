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

#include "obs_util.hpp"
#include "lang.h"
#include <algorithm>
#include <uiohook.h>

std::string util_file_filter(const char *display, const char *formats)
{
	std::string filter = display;
	filter += " (";
	filter += formats;
	filter += ");;";
	filter += T_FILTER_ALL_FILES;
	filter += " (*.*)";

	return filter;
}

void util_format_path(std::string &path)
{
	std::replace(path.begin(), path.end(), '\\', '/');
	const auto slash = strrchr(path.c_str(), '/');
	if (slash)
		path.resize(slash - path.c_str() + 1);
}
