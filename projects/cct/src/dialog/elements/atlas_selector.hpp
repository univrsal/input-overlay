/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2021 univrsal <uni@vrsal.de>.
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

#include "../../util/coordinate_system.hpp"
#include "../../util/texture.hpp"
#include "gui_element.hpp"
#include <SDL.h>

class sdl_helper;

class coordinate_system;

class texture;

class atlas_selector : public gui_element {
public:
    atlas_selector(int x, int y, int w, int h, texture *t, dialog *parent);

    ~atlas_selector();

    void close() override;

    void init(dialog *parent, SDL_Rect dim) override;

    void draw_foreground() override;

    void draw_background() override;

    bool handle_events(SDL_Event *event, bool was_handled) override;

    bool selection_changing() const { return m_cs->changing_selection(); }

    uint8_t get_cursor() override;

    void set_selection(SDL_Rect *r) const { m_cs->set_selection(r); }

private:
    void resize() override;

    texture *m_atlas; /* Is loaded in config, and will be deleted there */
    coordinate_system *m_cs{};
};
