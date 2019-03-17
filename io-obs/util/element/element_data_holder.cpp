/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "element_data_holder.hpp"
#include "element_trigger.hpp"
#include "sources/input_history.hpp"
#include "element_button.hpp"
#include "element_analog_stick.hpp"

element_data_holder::element_data_holder()
{
    m_button_data = std::map<uint16_t, std::unique_ptr<element_data>>();
}

element_data_holder::~element_data_holder()
{
    clear_data();
}

bool element_data_holder::is_empty() const
{
    auto flag = true;

    for (const auto& pad : m_gamepad_data)
    {
        if (!pad.empty())
        {
            flag = false;
            break;
        }
    }

    return flag && m_button_data.empty();
}

void element_data_holder::add_data(const uint16_t keycode, element_data* data)
{
    if (data_exists(keycode))
    {
        if (m_button_data[keycode]->is_persistent())
        {
            m_button_data[keycode]->merge(data);
            delete data; /* Existing data was used -> delete other one */
        }
        else
        {
            remove_data(keycode);
            m_button_data[keycode] = std::unique_ptr<element_data>(data);
        }
    }
    else
    {
        m_button_data[keycode] = std::unique_ptr<element_data>(data);
    }
}

void element_data_holder::add_gamepad_data(const uint8_t gamepad,
    const uint16_t keycode,
    element_data* data)
{
    if (gamepad_data_exists(gamepad, keycode))
    {
        if (m_gamepad_data[gamepad][keycode]->is_persistent())
        {
            m_gamepad_data[gamepad][keycode]->merge(data);

            delete data; /* Existing data was used -> delete other one */
        }
        else
        {
            remove_gamepad_data(gamepad, keycode);
            m_gamepad_data[gamepad][keycode] = std::unique_ptr<element_data>(data);
        }
    }
    else
    {
        m_gamepad_data[gamepad][keycode] = std::unique_ptr<element_data>(data);
    }
}

bool element_data_holder::gamepad_data_exists(const uint8_t gamepad, const uint16_t keycode)
{
    if (is_empty())
        return false;
    return m_gamepad_data[gamepad][keycode] != nullptr;
}

void element_data_holder::remove_gamepad_data(const uint8_t gamepad, const uint16_t keycode)
{
    if (gamepad_data_exists(gamepad, keycode))
    {
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

bool inline is_new_key(const std::vector<uint16_t>& vec, uint16_t vc)
{
    for (const auto& k : vec)
        if (k == vc)
            return false;
    return true;
}

void element_data_holder::populate_vector(std::vector<uint16_t>& vec, sources::history_settings* settings)
{
    for (const auto& data : m_button_data)
    {
        /* Mouse data is persistent and shouldn't be included
         * Any mouse buttons should only be included if enabled
         */
        if (data.first == VC_MOUSE_DATA || (data.first >> 8) == (VC_MOUSE_MASK >> 8)
            && !(settings->flags & sources::FLAG_INCLUDE_MOUSE))
            continue;
        
        blog(LOG_INFO, "vc: 0x%X", data.first);

        if (data.second->get_type() == BUTTON)
        {
            if (dynamic_cast<element_data_button*>(data.second.get())
                ->get_state() == STATE_RELEASED)
                continue;
        }

        if (is_new_key(vec, data.first)) /* if not add it */
        {
            vec.emplace_back(data.first);
        }
    }

    /* Same procedure for the gamepad */
    if (settings->flags & sources::FLAG_INCLUDE_PAD)
    {
        for (const auto& data : m_gamepad_data[settings->target_gamepad])
        {
            auto add = true;
            auto code = data.first;
            element_data_analog_stick* stick = nullptr;
            element_data_button* button = nullptr;
            element_data_trigger* trigger = nullptr;

            if (data.second)
            {
                switch (data.second->get_type())
                {
                case BUTTON:
                    button = dynamic_cast<element_data_button*>(data.second.get());
                    if (button && button->get_state() == STATE_RELEASED)
                        continue;
                    break;
                case ANALOG_STICK:
                    stick = dynamic_cast<element_data_analog_stick*>(data.second.get());
                    if (stick)
                    {
                        if (stick->left_pressed() && is_new_key(vec, PAD_L_ANALOG | VC_PAD_MASK))
                            vec.emplace_back(PAD_L_ANALOG | VC_PAD_MASK);
                        
                        if (stick->right_pressed() && is_new_key(vec, PAD_R_ANALOG | VC_PAD_MASK))
                            vec.emplace_back(PAD_R_ANALOG | VC_PAD_MASK);
                        add = false;
                    }
                    break;
                case TRIGGER:
                    trigger = dynamic_cast<element_data_trigger*>(data.second.get());

                    if (trigger)
                    {
                        if (trigger->get_left() > TRIGGER_THRESHOLD && is_new_key(vec, PAD_LT | VC_PAD_MASK))
                            vec.emplace_back(PAD_LT | VC_PAD_MASK);

                        if (trigger->get_right() > TRIGGER_THRESHOLD && is_new_key(vec, PAD_RT | VC_PAD_MASK))
                            vec.emplace_back(PAD_RT | VC_PAD_MASK);
                        add = false;
                    }
                default:;
                }
            }
            
            if (add && is_new_key(vec, code))
            {
                switch(code)
                {
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
}

bool element_data_holder::data_exists(const uint16_t keycode)
{
    if (is_empty())
        return false;
    
    return m_button_data[keycode] != nullptr;
}

void element_data_holder::remove_data(const uint16_t keycode)
{
    if (data_exists(keycode))
    {

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
