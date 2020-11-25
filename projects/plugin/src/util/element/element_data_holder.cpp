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

#include "element_data_holder.hpp"
#include "element_analog_stick.hpp"
#include "element_button.hpp"
#include "element_mouse_wheel.hpp"
#include "element_trigger.hpp"
#include <algorithm>
#include <keycodes.h>
#include <util/platform.h>

element_data_holder::element_data_holder(bool is_local)
{
	m_local = is_local;
	m_input_data = std::map<uint16_t, std::unique_ptr<element_data>>();
}

element_data_holder::~element_data_holder()
{
	clear_data();
}

bool element_data_holder::is_empty() const
{
	auto flag = true;

	//    for (const auto &pad : m_gamepad_data) {
	//        if (!pad.empty()) {
	//            flag = false;
	//            break;
	//        }
	//    }

	return flag && m_input_data.empty();
}

void element_data_holder::add_data(const uint16_t keycode, element_data *data)
{
	bool refresh = false;
	if (data_exists(keycode)) {
		refresh = m_input_data[keycode]->merge(data);
		delete data; /* Existing data was used -> delete other one */
	} else {
		m_input_data[keycode] = std::unique_ptr<element_data>(data);
		refresh = true;
	}

	if (refresh)
		m_last_input = os_gettime_ns();
}

//void element_data_holder::add_gamepad_data(const uint8_t gamepad, const uint16_t keycode, element_data *data)
//{
//    bool refresh = false;
//    if (gamepad_data_exists(gamepad, keycode)) {
//        refresh = m_gamepad_data[gamepad][keycode]->merge(data);
//        delete data; /* Existing data was used -> delete other one */
//    } else {
//        m_gamepad_data[gamepad][keycode] = std::unique_ptr<element_data>(data);
//        refresh = true;
//    }
//    if (refresh)
//        m_last_input = os_gettime_ns();
//}

//bool element_data_holder::gamepad_data_exists(const uint8_t gamepad, const uint16_t keycode)
//{
//    if (is_empty())
//        return false;
//    return m_gamepad_data[gamepad][keycode] != nullptr;
//}

//void element_data_holder::remove_gamepad_data(const uint8_t gamepad, const uint16_t keycode)
//{
//    if (gamepad_data_exists(gamepad, keycode)) {
//        m_gamepad_data[gamepad][keycode].reset();
//        m_gamepad_data[gamepad].erase(keycode);
//    }
//}

//element_data *element_data_holder::get_by_gamepad(const uint8_t gamepad, const uint16_t keycode)
//{
//    auto result = m_gamepad_data[gamepad][keycode].get();
//    return result;
//}

//void element_data_holder::clear_data()
//{
//    clear_button_data();
//    clear_gamepad_data();
//}

//void element_data_holder::clear_button_data()
//{
//    m_input_data.clear();
//}

//void element_data_holder::clear_gamepad_data()
//{
//    m_gamepad_data->clear();
//}

bool element_data_holder::is_local() const
{
	return m_local;
}

bool inline is_new_key(const std::vector<uint16_t> &vec, uint16_t vc)
{
	for (const auto &k : vec)
		if (k == vc)
			return false;
	return true;
}

bool element_data_holder::data_exists(const uint16_t keycode)
{
	if (is_empty())
		return false;

	return m_input_data[keycode] != nullptr;
}

uint64_t element_data_holder::get_last_input() const
{
	return m_last_input;
}

void element_data_holder::remove_data(const uint16_t keycode)
{
	if (data_exists(keycode)) {
		m_input_data[keycode].reset();
		m_input_data.erase(keycode);
	}
}

element_data *element_data_holder::get_by_code(const uint16_t keycode)
{
	if (!data_exists(keycode))
		return nullptr;

	return m_input_data[keycode].get();
}

void element_data_holder::clear_data()
{
	m_input_data.clear();
}
