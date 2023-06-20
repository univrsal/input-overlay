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

#include "element_trigger.hpp"
#include "../../sources/input_source.hpp"
#include <keycodes.h>
#include "SDL.h"

element_trigger::element_trigger() : element_texture(ET_TRIGGER) {}

void element_trigger::load(const QJsonObject &obj)
{
    element_texture::load(obj);
    m_button_mode = obj[CFG_TRIGGER_MODE].toBool();
    m_side = static_cast<element_side>(obj[CFG_SIDE].toInt());
    m_keycode = VC_TRIGGER_DATA;
    m_pressed = m_mapping;
    m_pressed.y = m_mapping.y + m_mapping.cy + CFG_INNER_BORDER;
    if (!m_button_mode)
        m_direction = static_cast<direction>(obj[CFG_DIRECTION].toInt());
}

void element_trigger::draw(gs_effect_t *effect, gs_image_file_t *image, sources::overlay_settings *settings)
{
    auto progress = 0.f;

    if (settings->gamepad || settings->remote_input_data) {
        switch (m_side) {
        case element_side::LEFT:
            progress = settings->pad_axis(SDL_CONTROLLER_AXIS_TRIGGERLEFT);
            break;
        case element_side::RIGHT:
            progress = settings->pad_axis(SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
            break;
        default:;
        }
    }

    if (m_button_mode) {
        if (progress >= 0.1)
            element_texture::draw(effect, image, &m_pressed);
        else
            element_texture::draw(effect, image, &m_mapping);
    } else {
        auto crop = m_pressed;
        auto new_pos = m_pos;
        calculate_mapping(&crop, &new_pos, progress);
        element_texture::draw(effect, image, &m_mapping); /* Draw unpressed first */
        element_texture::draw(effect, image, &crop, &new_pos);
    }
}

void element_trigger::calculate_mapping(gs_rect *pressed, vec2 *pos, const float progress) const
{
    switch (m_direction) {
    case DIR_UP:
        pressed->cy = static_cast<int>(m_mapping.cy * progress);
        pressed->y = m_pressed.y + (m_mapping.cy - pressed->cy);
        pos->y += m_mapping.cy - pressed->cy;
        break;
    case DIR_DOWN:
        pressed->cy = static_cast<int>(m_mapping.cy * progress);
        break;
    case DIR_LEFT:
        pressed->cx = static_cast<int>(m_mapping.cx * progress);
        pressed->x = m_mapping.x + (m_mapping.cx - pressed->cx);
        pos->x += (m_mapping.cx - pressed->cx);
        break;
    case DIR_RIGHT:
        pressed->cx = static_cast<int>(m_mapping.cx * progress);
        break;
    case DIR_MAX:
    case DIR_NONE:;
    }
}
