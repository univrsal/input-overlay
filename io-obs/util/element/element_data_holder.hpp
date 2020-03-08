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

#include "element.hpp"
#include <map>
#include <memory>
#include <vector>

namespace sources {
struct history_settings;
}

/* Holds all input data for connected clients
 * and/or the local computer
 */
class element_data_holder {
public:
	element_data_holder(bool is_local = true);

	~element_data_holder();

	void add_data(uint16_t keycode, element_data *data);

	bool data_exists(uint16_t keycode);

	void remove_data(uint16_t keycode);

	element_data *get_by_code(uint16_t keycode);

	void add_gamepad_data(uint8_t gamepad, uint16_t keycode, element_data *data);

	bool gamepad_data_exists(uint8_t gamepad, uint16_t keycode);

	void remove_gamepad_data(uint8_t gamepad, uint16_t keycode);

	element_data *get_by_gamepad(uint8_t gamepad, uint16_t keycode);

	void clear_data();

	void clear_button_data();

	void clear_gamepad_data();

	void populate_vector(std::vector<uint16_t> &vec, sources::history_settings *settings);

	bool is_empty() const;

	bool is_local() const;

	uint64_t get_last_input() const;

private:
	/* Used to check if new inputs happened
     * in input history */
	uint64_t m_last_input = 0;
	bool m_local; /* True if this holds the data for the local pc */
	std::map<uint16_t, std::unique_ptr<element_data>> m_button_data;
	std::map<uint16_t, std::unique_ptr<element_data>> m_gamepad_data[4];
};
