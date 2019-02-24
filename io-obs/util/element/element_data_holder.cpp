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
    auto flag = false;

    for (const auto& pad : m_gamepad_data)
    {
        if (pad.empty())
        {
            flag = true;
            break;
        }
    }

    return m_button_data.empty() && flag;
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

void element_data_holder::populate_vector(std::vector<uint16_t>& vec, sources::history_settings* settings)
{
    for (const auto& data : m_button_data)
    {
        auto new_key = true;
        /* Mouse data is persistent and shouldn't be included
         * Any mouse buttons should only be included if enabled
         */
        if (data.first == VC_MOUSE_DATA || (data.first >> 8) == (VC_MOUSE_MASK >> 8)
            && !(settings->flags & sources::FLAG_INCLUDE_MOUSE))
            continue;

        if (data.second->get_type() == BUTTON)
        {
            if (dynamic_cast<element_data_button*>(data.second.get())
                ->get_state() == STATE_RELEASED)
                continue;
        }

        for (const auto& vc : vec) /* Check if this key is already in the vector */
        {
            if (data.first == vc)
            {
                new_key = false;
                break;
            }
        }

        if (new_key) /* if not add it */
        {
            vec.emplace_back(data.first);
        }
    }

    /* Same procedure for the gamepad */
    if (settings->flags & sources::FLAG_INCLUDE_PAD)
    {
        for (const auto& data : m_gamepad_data[settings->target_gamepad])
        {
            auto new_key = true;
            for (const auto& vc : vec) /* Check if this key is already in the vector */
            {
                if (data.first == vc)
                {
                    new_key = false;
                    break;
                }
            }

            if (new_key) /* if not add it */
            {
                vec.emplace_back(data.first);
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
