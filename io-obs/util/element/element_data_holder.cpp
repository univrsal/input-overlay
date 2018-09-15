/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "element_data_holder.hpp"


element_data_holder::element_data_holder()
{
    m_data = std::map<uint16_t, std::unique_ptr<element_data>>();
}

element_data_holder::~element_data_holder()
{
    m_data.clear();
    m_map_cleared = true;
}

bool element_data_holder::is_empty() const
{
    bool flag = true;

    for (auto pad : m_gamepad_data)
    {
        if (pad.empty)
        {
            flag = false;
            break;
        }
    }

    return (m_map_cleared || m_data.empty()) && flag;
}

void element_data_holder::add_data(const uint16_t keycode, element_data* data)
{
    if (data_exists(keycode))
    {
        if (m_data[keycode]->is_presistent())
        {
            m_data[keycode]->merge(data);
#ifdef _DEBUG
            blog(LOG_INFO, "Merged keycode 0x%X", keycode);
#endif
            delete data; /* Existing data was used -> delete other one */
        }
        else
        {
            remove_data(keycode);
            m_data[keycode] = std::unique_ptr<element_data>(data);
#ifdef _DEBUG
            blog(LOG_INFO, "Replaced keycode 0x%X", keycode);
#endif    
        }
    }
    else
    {
        m_data[keycode] = std::unique_ptr<element_data>(data);
    }
}

void element_data_holder::add_gamepad_data(uint8_t gamepad, uint16_t keycode, element_data * data)
{
    if (gamepad_data_exists(gamepad, keycode))
    {
        if (m_gamepad_data[gamepad][keycode]->is_presistent())
        {
            m_gamepad_data[gamepad][keycode]->merge(data);
#ifdef _DEBUG
            blog(LOG_INFO, "Merged keycode 0x%X", keycode);
#endif
            delete data; /* Existing data was used -> delete other one */
        }
        else
        {
            remove_gamepad_data(gamepad, keycode);
            m_gamepad_data[gamepad][keycode] = std::unique_ptr<element_data>(data);
#ifdef _DEBUG
            blog(LOG_INFO, "Replaced keycode 0x%X", keycode);
#endif    
        }
    }
    else
    {
        m_gamepad_data[gamepad][keycode] = std::unique_ptr<element_data>(data);
    }
}

bool element_data_holder::gamepad_data_exists(uint8_t gamepad, uint16_t keycode)
{
    if (is_empty())
        return false;
    return m_gamepad_data[gamepad][keycode] != nullptr;
}

void element_data_holder::remove_gamepad_data(uint8_t gamepad, uint16_t keycode)
{
    if (gamepad_data_exists(gamepad, keycode))
    {
        m_gamepad_data[gamepad][keycode].reset(nullptr);
        m_gamepad_data[gamepad].erase(keycode);
#ifdef _DEBUG
        blog(LOG_INFO, "REMOVED keycode 0x%X, size: %i", keycode, m_data.size());
#endif
    }
}

element_data * element_data_holder::get_by_gamepad(uint8_t gamepad, uint16_t keycode)
{
    return m_gamepad_data[gamepad][keycode].get();
}

bool element_data_holder::data_exists(const uint16_t keycode)
{
    if (is_empty())
        return false;
    return m_data[keycode] != nullptr;
}

void element_data_holder::remove_data(const uint16_t keycode)
{
    if (data_exists(keycode))
    {
        m_data[keycode].reset(nullptr);
        m_data.erase(keycode);
#ifdef _DEBUG
        blog(LOG_INFO, "REMOVED keycode 0x%X, size: %i", keycode, m_data.size());
#endif
    }
}

element_data* element_data_holder::get_by_code(const uint16_t keycode)
{
    return m_data[keycode].get();
}
