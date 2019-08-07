/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#include "element_data_holder.hpp"
#include "element_trigger.hpp"
#include "sources/input_history.hpp"
#include "element_button.hpp"
#include "element_analog_stick.hpp"
#include "element_mouse_wheel.hpp"
#include <util/platform.h>
#include <algorithm>

element_data_holder::element_data_holder(bool is_local)
{
    m_local = is_local;
    m_button_data = std::map<uint16_t, std::unique_ptr<element_data>>();
}

element_data_holder::~element_data_holder()
{
    clear_data();
}

bool element_data_holder::is_empty() const
{
    auto flag = true;

    for (const auto &pad : m_gamepad_data) {
        if (!pad.empty()) {
            flag = false;
            break;
        }
    }

    return flag && m_button_data.empty();
}

void element_data_holder::add_data(const uint16_t keycode, element_data* data)
{
    bool refresh = false;
    if (data_exists(keycode)) {
        refresh = m_button_data[keycode]->merge(data);
        delete data; /* Existing data was used -> delete other one */
    } else {
        m_button_data[keycode] = std::unique_ptr<element_data>(data);
        refresh = true;
    }
    if (refresh)
        m_last_input = os_gettime_ns();
}

void element_data_holder::add_gamepad_data(const uint8_t gamepad, const uint16_t keycode, element_data* data)
{
    bool refresh = false;
    if (gamepad_data_exists(gamepad, keycode)) {
        refresh = m_gamepad_data[gamepad][keycode]->merge(data);
        delete data; /* Existing data was used -> delete other one */
    } else {
        m_gamepad_data[gamepad][keycode] = std::unique_ptr<element_data>(data);
        refresh = true;
    }
    if (refresh)
        m_last_input = os_gettime_ns();
}

bool element_data_holder::gamepad_data_exists(const uint8_t gamepad, const uint16_t keycode)
{
    if (is_empty())
        return false;
    return m_gamepad_data[gamepad][keycode] != nullptr;
}

void element_data_holder::remove_gamepad_data(const uint8_t gamepad, const uint16_t keycode)
{
    if (gamepad_data_exists(gamepad, keycode)) {
        m_gamepad_data[gamepad][keycode].reset();
        m_gamepad_data[gamepad].erase(keycode);
    }
}

element_data* element_data_holder::get_by_gamepad(const uint8_t gamepad, const uint16_t keycode)
{
    auto result = m_gamepad_data[gamepad][keycode].get();
    return result;
}

void element_data_holder::clear_data()
{
    clear_button_data();
    clear_gamepad_data();
}

void element_data_holder::clear_button_data()
{
    m_button_data.clear();
}

void element_data_holder::clear_gamepad_data()
{
    m_gamepad_data->clear();
}

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

void element_data_holder::populate_vector(std::vector<uint16_t> &vec, sources::history_settings* settings)
{
    for (const auto &data : m_button_data) {
        if (!data.second)
            continue;
        /* Mouse data is persistent and shouldn't be included
         * Any mouse buttons should only be included if enabled
         * MOUSE_DATA is only used in mouse movement, so it'll
         * always be excluded
         */
        if (data.first == VC_MOUSE_DATA)
            continue;
        if ((data.first >> 8) == (VC_MOUSE_MASK >> 8) && !(settings->flags & (int) sources::history_flags::INCLUDE_MOUSE))
            continue;

        if (data.second->get_type() == element_type::BUTTON) {
            if (dynamic_cast<element_data_button*>(data.second.get())->get_state() == button_state::RELEASED)
                continue;
        } else if (data.second->get_type() == element_type::MOUSE_SCROLLWHEEL) {
            auto* wheel = dynamic_cast<element_data_wheel*>(data.second.get());
            if (wheel) {
                if (wheel->get_dir() == wheel_direction::UP && is_new_key(vec, VC_MOUSE_WHEEL_UP))
                    vec.emplace_back(VC_MOUSE_WHEEL_UP);
                else if (wheel->get_dir() == wheel_direction::DOWN && is_new_key(vec, VC_MOUSE_WHEEL_DOWN))
                    vec.emplace_back(VC_MOUSE_WHEEL_DOWN);
                if (wheel->get_state() == button_state::PRESSED && is_new_key(vec, VC_MOUSE_WHEEL))
                    vec.emplace_back(VC_MOUSE_WHEEL);
                continue;
            }
        }

        if (is_new_key(vec, data.first)) /* if not add it */
            vec.emplace_back(data.first);
    }

    /* Same procedure for the gamepad */
    if (settings->flags & (int) sources::history_flags::INCLUDE_PAD) {
        for (const auto &data : m_gamepad_data[settings->target_gamepad]) {
            auto add = true;
            auto code = data.first;
            element_data_analog_stick* stick = nullptr;
            element_data_button* button = nullptr;
            element_data_trigger* trigger = nullptr;

            if (data.second) {
                switch (data.second->get_type()) {
                    case element_type::BUTTON:
                        button = dynamic_cast<element_data_button*>(data.second.get());
                        if (button && button->get_state() == button_state::RELEASED)
                            continue;
                        break;
                    case element_type::ANALOG_STICK:
                        stick = dynamic_cast<element_data_analog_stick*>(data.second.get());
                        if (stick) {
                            if (stick->left_pressed() && is_new_key(vec, VC_PAD_L_ANALOG))
                                vec.emplace_back(VC_PAD_L_ANALOG);

                            if (stick->right_pressed() && is_new_key(vec, VC_PAD_R_ANALOG))
                                vec.emplace_back(VC_PAD_R_ANALOG);
                            add = false;
                        }
                        break;
                    case element_type::TRIGGER:
                        trigger = dynamic_cast<element_data_trigger*>(data.second.get());

                        if (trigger) {
                            if (trigger->get_left() > TRIGGER_THRESHOLD && is_new_key(vec, VC_PAD_LT))
                                vec.emplace_back(VC_PAD_LT);

                            if (trigger->get_right() > TRIGGER_THRESHOLD && is_new_key(vec, VC_PAD_RT))
                                vec.emplace_back(VC_PAD_RT);
                            add = false;
                        }
                }
            }

            if (add && is_new_key(vec, code)) {
                switch (code) {
                    case VC_STICK_DATA:
                    case VC_TRIGGER_DATA:
                    case VC_DPAD_DATA:
                        break;
                    default:
                        vec.emplace_back(code);
                }
            }
        }
    }
    /* Sort the vector, so keycombinations are always displayed the same way
     * Sorting is done in reverse so modifier keys like shift are at the
     * beginning */
    std::sort(vec.rbegin(), vec.rend());
}

bool element_data_holder::data_exists(const uint16_t keycode)
{
    if (is_empty())
        return false;

    return m_button_data[keycode] != nullptr;
}

uint64_t element_data_holder::get_last_input() const
{
    return m_last_input;
}

void element_data_holder::remove_data(const uint16_t keycode)
{
    if (data_exists(keycode)) {
        m_button_data[keycode].reset();
        m_button_data.erase(keycode);
    }
}

element_data* element_data_holder::get_by_code(const uint16_t keycode)
{
    if (!data_exists(keycode))
        return nullptr;

    return m_button_data[keycode].get();
}
