/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.xyz/alex/input-overlay
 * Copyright 2023 univrsal <uni@vrsal.xyz>.
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

#include <QString>
#include <unordered_map>
#include <string>
#include <memory>
#include <mutex>
#include <input_data.hpp>

extern "C" {
#include <mongoose.h>
}

namespace network {
/* Set in obs_module_load */
extern bool local_input;
extern std::mutex remote_data_map_mutex;
extern std::unordered_map<std::string, std::shared_ptr<input_data>> remote_data;
QString get_local_ip();

extern void process_remote_event(struct mg_connection *ws, unsigned char *bytes, size_t len);
}
