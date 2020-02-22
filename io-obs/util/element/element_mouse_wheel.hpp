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
#include <layout_constants.h>

#define WHEEL_MAP_MIDDLE 0
#define WHEEL_MAP_UP 1
#define WHEEL_MAP_DOWN 2

enum wheel_data {
    WD_BOTH,
    WD_BUTTON,
    WD_WHEEL
};

class element_data_wheel : public element_data {
public:
    element_data_wheel(direction dir, button_state state);

    explicit element_data_wheel(direction dir);

    explicit element_data_wheel(button_state state = BS_PRESSED);

    direction get_dir() const;

    void set_dir(direction dir);

    button_state get_state() const;

    bool is_persistent() override;

    bool merge(element_data* other) override;

    wheel_data get_data_type() const;

private:
    wheel_data m_data_type;
    button_state m_middle_button;
    direction m_dir;
};

class element_wheel : public element_texture {
public:
    element_wheel();

    void load(const QJsonObject& obj) override;

    void
    draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data, sources::overlay_settings* settings) override;

    data_source get_source() override;

private:
    /* Middle, Up, Down */
    gs_rect m_mappings[3];
};
