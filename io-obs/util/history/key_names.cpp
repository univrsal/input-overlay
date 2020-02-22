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

#include "key_names.hpp"
#include "../../../ccl/ccl.hpp"
#include <obs-module.h>

void key_names::load_from_file(const char *path)
{
	m_names.clear();
	auto cfg = ccl_config(path, "");

	if (!cfg.is_empty()) {
		auto node = cfg.get_first();

		if (!node)
			return;

		do {
			if (node->get_type() == ccl_type_string) {
				auto val = node->get_id();
				uint16_t key_code = std::stoul(val, nullptr, 16);
				m_names[key_code] = node->get_value();
			}
		} while ((node = node->get_next()) != nullptr);
	}

	if (cfg.has_errors())
		blog(LOG_WARNING, "[input-overlay] %s", cfg.get_error_message().c_str());
}

bool key_names::empty() const
{
	return m_names.empty();
}

const char *key_names::get_name(const uint16_t vc)
{
	/* Users can use an empty name in the config to
     * prevent certain keys from showing up in input-history
     * empty name = key is disabled
     */
	if (m_names.find(vc) != m_names.end() && !m_names[vc].empty())
		return m_names[vc].c_str();

	return nullptr;
}
