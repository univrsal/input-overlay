/**
 * This file is part of obs-studio
 * which is licensed under the GPL 2.0
 * See COPYING or https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * https://github.com/obsproject/obs-studio/blob/master/UI/frontend-plugins/frontend-tools/auto-scene-switcher.hpp
 */

#pragma once

#include <string>
#include <vector>

void GetWindowList(std::vector<std::string> &windows);

void GetCurrentWindowTitle(std::string &title);
