/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2019 univrsal <universailp@web.de>.
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

enum trigger_data
{
    TD_NONE = -1,
    TD_BOTH,
    TD_LEFT,
    TD_RIGHT
};

/* Contains data for both trigger buttons
 */
class element_data_trigger : public element_data
{
public:
    /*
        Separate constructors are used on linux
        because the values can't be queried together
    */
    element_data_trigger(trigger_data side, float val);

    element_data_trigger(float left, float right);

    float get_left() const;

    float get_right() const;

    bool is_persistent() override;

    bool merge(element_data* other) override;

    static element_data_trigger* from_buffer(netlib_byte_buf* buffer);

private:
    trigger_data m_data_type = TD_BOTH;
    float m_left_trigger = 0.f, m_right_trigger = 0.f;
};

class element_trigger : public element_texture
{
public:
    element_trigger();

    void load(ccl_config* cfg, const std::string& id) override;

    void draw(gs_effect_t* effect, gs_image_file_t* image,
        element_data* data, sources::overlay_settings* settings) override;

    data_source get_source() override;

private:
    void calculate_mapping(gs_rect* pressed, vec2* pos, float progress) const;
    gs_rect m_pressed;
    element_side m_side;
    direction m_direction;
    bool m_button_mode = false;
};
