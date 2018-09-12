/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "element_data_holder.hpp"


element_data_holder::~element_data_holder()
{
    m_data.clear();
    m_map_cleared = true;
}

bool element_data_holder::is_empty() const
{
    return m_map_cleared || m_data.empty();
}

void element_data_holder::add_data(const uint16_t keycode, element_data* data)
{
    if (data_exists(keycode))
    {
        if (m_data[keycode]->is_presistent())
        {
            m_data[keycode]->merge(data);
            delete data;
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
        blog(LOG_INFO, "REMOVED keycode 0x%X, size: %i\n", keycode, m_data.size());
#endif
    }
}

element_data* element_data_holder::get_by_code(const uint16_t keycode)
{
    if (data_exists(keycode))
        return m_data[keycode].get();
    return nullptr;
}
