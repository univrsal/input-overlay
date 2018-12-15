/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "element_data_holder.hpp"
#include "element_trigger.hpp"

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
    auto flag = true;

    for (const auto& pad : m_gamepad_data)
    {
        if (pad.empty())
        {
            flag = false;
            break;
        }
    }

    return (m_map_cleared || m_data.empty()) && flag;
}

bool element_data_holder::is_blocked() const
{
    return m_data_locked || m_gamepad_data_locked;
}

void element_data_holder::add_data(const uint16_t keycode, element_data* data)
{
#ifdef _DEBUG
    blog(LOG_INFO, "Incoming: 0x%4X\n", keycode);
#endif

    m_data_locked = true;
    if (data_exists(keycode))
    {
        if (m_data[keycode]->is_persistent())
        {
            m_data[keycode]->merge(data);
            delete data; /* Existing data was used -> delete other one */
        }
        else
        {
            remove_data(keycode);
            m_data[keycode] = std::unique_ptr<element_data>(data);
        }
    }
    else
    {
        m_data[keycode] = std::unique_ptr<element_data>(data);
    }
    m_data_locked = false;
}

void element_data_holder::add_gamepad_data(const uint8_t gamepad,
    const uint16_t keycode,
    element_data* data)
{
    m_gamepad_data_locked = true;
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
    m_gamepad_data_locked = false;
}

bool element_data_holder::gamepad_data_exists(const uint8_t gamepad, const uint16_t keycode)
{
    if (is_empty())
        return false;
    return m_gamepad_data[gamepad][keycode] != nullptr;
}

void element_data_holder::remove_gamepad_data(const uint8_t gamepad, const uint16_t keycode)
{
    m_gamepad_data_locked = true;
    if (gamepad_data_exists(gamepad, keycode))
    {
        m_gamepad_data[gamepad][keycode].reset();
        m_gamepad_data[gamepad].erase(keycode);
    }
    m_gamepad_data_locked = false;
}

element_data* element_data_holder::get_by_gamepad(const uint8_t gamepad, const uint16_t keycode)
{
    if (m_gamepad_data_locked)
    {
#ifdef _DEBUG
        blog(LOG_INFO, "Suppressed game pad access, data is locked");
#endif
        return nullptr;
    }
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
    m_data_locked = true;
    if (data_exists(keycode))
    {
        m_data[keycode].reset();
        m_data.erase(keycode);
    }
    m_data_locked = false;
}

element_data* element_data_holder::get_by_code(const uint16_t keycode)
{
    if (m_data_locked || m_data.empty())
    {
#ifdef _DEBUG
        blog(LOG_INFO, "Suppressed input access, data is locked");
#endif
        return nullptr;
    }
    return m_data[keycode].get();
}
