/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.xyz/alex/input-overlay
 * Copyright 2023 univrsal <uni@vrsal.xyz>.
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

#include "../../sources/input_source.hpp"
#include "element_dpad.hpp"
#include <keycodes.h>
#include "SDL.h"

element_dpad::element_dpad() : element_texture(ET_DPAD_STICK) {}

void element_dpad::load(const QJsonObject &obj)
{
    element_texture::load(obj);
    auto i = 1;
    for (auto &map : m_mappings) {
        map = m_mapping;
        map.x += i * (m_mapping.cx + CFG_INNER_BORDER);
        i++;
    }
    m_keycode = VC_DPAD_DATA;
}

inline int get_direction(std::array<bool, SDL_CONTROLLER_BUTTON_MAX> const &buttons)
{
    if (buttons[SDL_CONTROLLER_BUTTON_DPAD_UP]) {
        if (buttons[SDL_CONTROLLER_BUTTON_DPAD_LEFT])
            return element_dpad::TEXTURE_TOP_LEFT;
        if (buttons[SDL_CONTROLLER_BUTTON_DPAD_RIGHT])
            return element_dpad::TEXTURE_TOP_RIGHT;
        return element_dpad::TEXTURE_UP;
    } else if (buttons[SDL_CONTROLLER_BUTTON_DPAD_DOWN]) {
        if (buttons[SDL_CONTROLLER_BUTTON_DPAD_LEFT])
            return element_dpad::TEXTURE_BOTTOM_LEFT;
        if (buttons[SDL_CONTROLLER_BUTTON_DPAD_RIGHT])
            return element_dpad::TEXTURE_BOTTOM_RIGHT;
        return element_dpad::TEXTURE_DOWN;
    } else if (buttons[SDL_CONTROLLER_BUTTON_DPAD_RIGHT]) {
        if (buttons[SDL_CONTROLLER_BUTTON_DPAD_UP])
            return element_dpad::TEXTURE_TOP_RIGHT;
        if (buttons[SDL_CONTROLLER_BUTTON_DPAD_DOWN])
            return element_dpad::TEXTURE_BOTTOM_RIGHT;
        return element_dpad::TEXTURE_RIGHT;
    } else if (buttons[SDL_CONTROLLER_BUTTON_DPAD_LEFT]) {
        if (buttons[SDL_CONTROLLER_BUTTON_DPAD_UP])
            return element_dpad::TEXTURE_TOP_LEFT;
        if (buttons[SDL_CONTROLLER_BUTTON_DPAD_DOWN])
            return element_dpad::TEXTURE_BOTTOM_LEFT;
        return element_dpad::TEXTURE_LEFT;
    }
    return -1;
}

void element_dpad::draw(gs_effect_t *effect, gs_image_file_t *image, sources::overlay_settings *settings)
{
    auto dir = 0;
    if (settings->gamepad)
        dir = get_direction(settings->get_button_map());

    if (dir >= 0) {
        /* Enum starts at one (Center doesn't count)*/
        const auto map = &m_mappings[dir];
        element_texture::draw(effect, image, map);
    } else {
        element_texture::draw(effect, image, nullptr);
    }
}
