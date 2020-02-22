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

#include "../tool.hpp"
#include "../util/constants.hpp"
#include "../util/sdl_helper.hpp"
#include "dialog.hpp"
#include "elements/button.hpp"
#include "elements/textbox.hpp"

class sdl_helper;

class tool;

class element;

class dialog_element_settings : public dialog {
public:
    dialog_element_settings(sdl_helper* sdl, tool* tool)
        : dialog(sdl, SDL_Rect { 1030, 200, 240, 400 },
            LANG_DIALOG_ELEMENT_SETTINGS)
    {
        m_tool = tool;
    };

    void init() override;

    void action_performed(int8_t action_id) override;

    void set_wh(int w, int h) const;

    void set_xy(int x, int y) const;

    void set_uv(int u, int v) const;

    void set_id(std::string id) const;

    void set_vc(uint16_t vc) const;

    void set_z_level(uint8_t z) const;

    void select_element(element* e) const;

    int get_x() const
    {
        return SDL_atoi(m_element_x->c_str());
    }

    int get_y() const
    {
        return SDL_atoi(m_element_y->c_str());
    }

    int get_z() const
    {
        return SDL_atoi(m_element_z_level->c_str());
    }

    int get_u() const
    {
        return SDL_atoi(m_element_u->c_str());
    }

    int get_v() const
    {
        return SDL_atoi(m_element_v->c_str());
    }

    int get_w() const
    {
        return SDL_atoi(m_element_width->c_str());
    }

    int get_h() const
    {
        return SDL_atoi(m_element_height->c_str());
    }

    uint16_t get_vc() const
    {
        return SDL_strtol(m_element_vc->c_str(), nullptr, 16);
    }

    SDL_Rect get_mapping() const
    {
        return { get_u(), get_v(), get_w(), get_h() };
    }

    const std::string* get_id() const
    {
        return m_element_id->get_text();
    }

    void alert_element_id() const
    {
        m_element_id->set_alert(true);
    }

private:
    bool m_finished = false;

    textbox* m_element_id = nullptr;
    textbox* m_element_vc = nullptr;
    textbox* m_element_width = nullptr;
    textbox* m_element_height = nullptr;
    textbox* m_element_x = nullptr;
    textbox* m_element_y = nullptr;
    textbox* m_element_u = nullptr;
    textbox* m_element_v = nullptr;
    textbox* m_element_z_level = nullptr;

    tool* m_tool = nullptr;
};
