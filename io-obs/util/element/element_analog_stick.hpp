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

#include "element_texture.hpp"
#include <netlib.h>

enum stick_data_type {
    SD_BOTH,
    SD_PRESSED_STATE_LEFT,
    SD_PRESSED_STATE_RIGHT,
    SD_LEFT_X,
    SD_LEFT_Y,
    SD_RIGHT_X,
    SD_RIGHT_Y
};

/* Contains data for both analog sticks
 */
class element_data_analog_stick : public element_data {
public:
    element_data_analog_stick()
        : element_data(ET_ANALOG_STICK)
        , m_left_stick()
        , m_right_stick()
        , m_left_state()
        , m_right_state()
    {
        m_data_type = SD_BOTH;
    }

    /*
        Separate constructors are used on linux
        because the values can't be queried together
    */
    element_data_analog_stick(const button_state state,
        const element_side side)
        : element_data(ET_ANALOG_STICK)
        , m_left_stick()
        , m_right_stick()
    {
        if (side == ES_LEFT) {
            m_left_state = state;
            m_data_type = SD_PRESSED_STATE_LEFT;
        } else {
            m_right_state = state;
            m_data_type = SD_PRESSED_STATE_RIGHT;
        }
    }

    element_data_analog_stick(const float axis_value,
        const stick_data_type data_type)
        : element_data(ET_ANALOG_STICK)
        , m_left_state()
        , m_right_state()
    {
        switch (data_type) {
        case SD_LEFT_X:
            m_left_stick = { axis_value, -1 };
            break;
        case SD_LEFT_Y:
            m_left_stick = { -1, axis_value };
            break;
        case SD_RIGHT_X:
            m_right_stick = { axis_value, -1 };
            break;
        case SD_RIGHT_Y:
            m_right_stick = { -1, axis_value };
            break;
        default:;
        }
        m_data_type = data_type;
    }

    element_data_analog_stick(bool left, bool right, const float l_x, const float l_y,
        const float r_x, const float r_y)
        : element_data(ET_ANALOG_STICK)
    {
        m_left_stick = { l_x, l_y };
        m_right_stick = { r_x, r_y };
        m_left_state = static_cast<button_state>(left);
        m_right_state = static_cast<button_state>(right);
        m_data_type = SD_BOTH;
    }

    bool left_pressed() const
    {
        return m_left_state == BS_PRESSED;
    }

    bool right_pressed() const
    {
        return m_right_state == BS_PRESSED;
    }

    const vec2* get_left_stick() const
    {
        return &m_left_stick;
    }

    const vec2* get_right_stick() const
    {
        return &m_right_stick;
    }

    void set_state(button_state left, button_state right);

    bool is_persistent() override
    {
        return true;
    }

    bool merge(element_data* other) override;

    static element_data_analog_stick* from_buffer(netlib_byte_buf* buffer);

private:
    vec2 m_left_stick {}, m_right_stick {};
    stick_data_type m_data_type = SD_BOTH;
    button_state m_left_state, m_right_state;
};

class element_analog_stick : public element_texture {
public:
    element_analog_stick()
        : element_texture(ET_ANALOG_STICK)
        , m_side()
    {
    }

    void load(const QJsonObject& obj) override;

    void
    draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data, sources::overlay_settings* settings) override;

    data_source get_source() override
    {
        return DS_GAMEPAD;
    }

private:
    void calc_position(vec2* v, element_data_analog_stick* d, sources::overlay_settings* settings) const;

    gs_rect m_pressed {};
    element_side m_side;
    uint8_t m_radius = 0;
};
