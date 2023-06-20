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

#pragma once

#include "element_texture.hpp"
#include <layout_constants.h>

class element_button : public element_texture {
public:
    explicit element_button(element_type t) : element_texture(t), m_pressed() {}

    void load(const QJsonObject &objc) override;
    void draw(gs_effect_t *effect, gs_image_file_t *image, sources::overlay_settings *settings) override
    {
        element_texture::draw(effect, image, settings);
    }

protected:
    gs_rect m_pressed;
};

class element_keyboard_key : public element_button {
public:
    element_keyboard_key() : element_button(ET_KEYBOARD_KEY) {}

    void draw(gs_effect_t *effect, gs_image_file_t *image, sources::overlay_settings *settings) override;
};

class element_mouse_button : public element_button {
public:
    element_mouse_button() : element_button(ET_MOUSE_BUTTON) {}

    void load(const QJsonObject &objc) override;
    void draw(gs_effect_t *effect, gs_image_file_t *image, sources::overlay_settings *settings) override;
};

class element_gamepad_button : public element_button {
public:
    element_gamepad_button() : element_button(ET_GAMEPAD_BUTTON) {}

    void draw(gs_effect_t *effect, gs_image_file_t *image, sources::overlay_settings *settings) override;
};
