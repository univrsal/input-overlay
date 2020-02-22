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

#include "../util/util.hpp"
#include "element_texture.hpp"

#define WHEEL_RESET 150

#define POS_WHEEL_NONE 0
#define POS_WHEEL_PRESSED 1
#define POS_WHEEL_UP 2
#define POS_WHEEL_DOWN 3

class ElementScrollWheel : public element_texture {
public:
    ElementScrollWheel()
        : element_texture() {
            /* NO-OP */
        };

    ElementScrollWheel(const std::string& id, SDL_Point pos, SDL_Rect mapping, uint8_t z);

    void draw(texture* atlas, coordinate_system* cs, bool selected, bool alpha) override;

    void handle_event(SDL_Event* event, sdl_helper* helper) override;

    void refresh_mappings();

    void update_settings(dialog_new_element* dialog) override;

    void update_settings(dialog_element_settings* dialog) override;

    static ElementScrollWheel* read_from_file(ccl_config* file, const std::string& id, SDL_Point* default_dim);

private:
    SDL_Rect m_mapping_pressed = {}, m_mapping_down = {}, m_mapping_up = {};
    Timer m_wheel_reset;
    bool m_pressed = false;
    bool m_up = false;
    bool m_down = false;
};
