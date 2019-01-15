/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include "element.hpp"
#include <map>
#include <memory>

class element_data_holder
{
public:
    element_data_holder();
    ~element_data_holder();

    void add_data(uint16_t keycode, element_data* data);
    bool data_exists(uint16_t keycode);
    void remove_data(uint16_t keycode);
    element_data* get_by_code(uint16_t keycode);

    void add_gamepad_data(uint8_t gamepad, uint16_t keycode, element_data* data);
    bool gamepad_data_exists(uint8_t gamepad, uint16_t keycode);
    void remove_gamepad_data(uint8_t gamepad, uint16_t keycode);
    element_data* get_by_gamepad(uint8_t gamepad, uint16_t keycode);

    void clear_data();
    void clear_button_data();
    void clear_gamepad_data();

    bool is_empty() const;
    bool is_blocked() const;
private:
    std::map<uint16_t, std::unique_ptr<element_data>> m_data;
    std::map<uint16_t, std::unique_ptr<element_data>> m_gamepad_data[4];

    /* Prevent drawing thread from accessing data while
     * hook threads write to it
     * TODO: use mutex?
     */
    volatile bool m_data_locked = false;
    volatile bool m_gamepad_data_locked = false;
};
