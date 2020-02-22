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

#include "element_button.hpp"
#include "../../../ccl/ccl.hpp"
#include "../../../io-obs/util/util.hpp"
#include "../dialog/dialog_element_settings.hpp"
#include "../dialog/dialog_new_element.hpp"
#include "../util/coordinate_system.hpp"
#include "../util/notifier.hpp"
#include "../util/palette.hpp"
#include "../util/sdl_helper.hpp"
#include "../util/texture.hpp"

ElementButton::ElementButton(const std::string& id, const SDL_Point pos, const SDL_Rect mapping, const uint16_t vc,
    const uint8_t z)
    : element_texture(ET_BUTTON, id, pos, mapping, z)
{
    m_keycode = vc;
    m_pressed_mapping = m_mapping;
    m_pressed_mapping.y += m_mapping.h + CFG_INNER_BORDER;
}

element_error ElementButton::is_valid(notifier* n, sdl_helper* h)
{
    auto error = element_texture::is_valid(n, h);
    if (error == EE_VALID && m_keycode == 0x0) {
        n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_KEYCODE_INVALID));
        error = EE_KEYCODE_INVALID;
    }
    return error;
}

void ElementButton::draw(texture* atlas, coordinate_system* cs, const bool selected, const bool alpha)
{
    get_abs_dim(cs);
    if (m_pressed)
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_pressed_mapping,
            (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);
    else
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping,
            (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);

    if (selected)
        cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->get_palette()->red());
}

void ElementButton::write_to_file(ccl_config* cfg, SDL_Point* default_dim, uint8_t& layout_flags)
{
    element_texture::write_to_file(cfg, default_dim, layout_flags);
    const auto comment = "Key code of " + m_id;
    cfg->add_int(m_id + CFG_KEY_CODE, comment, m_keycode, true);

    if ((m_keycode >> 8) == (VC_PAD_MASK >> 8))
        layout_flags |= OF_GAMEPAD;
}

void ElementButton::update_settings(dialog_new_element* dialog)
{
    element_texture::update_settings(dialog);
    m_pressed_mapping = m_mapping;
    m_pressed_mapping.y += m_mapping.h + CFG_INNER_BORDER;
    m_keycode = dialog->get_vc();
}

void ElementButton::update_settings(dialog_element_settings* dialog)
{
    element_texture::update_settings(dialog);
    m_pressed_mapping = m_mapping;
    m_pressed_mapping.y += m_mapping.h + CFG_INNER_BORDER;
    m_keycode = dialog->get_vc();
}

void ElementButton::handle_event(SDL_Event* event, sdl_helper* helper)
{
    uint32_t button = SDLK_UNKNOWN;
    auto pressed = false;

    switch (event->type)
    /* Fallthrough intended */ {
    case SDL_KEYDOWN:
        pressed = true;
    case SDL_KEYUP:
        button = event->key.keysym.sym;
        break;
    case SDL_CONTROLLERBUTTONDOWN:
        pressed = true;
    case SDL_CONTROLLERBUTTONUP:
        button = TO_PAD_MASK(event->cbutton.button);
        break;
    case SDL_MOUSEBUTTONDOWN:
        pressed = true;
    case SDL_MOUSEBUTTONUP:
        button = TO_MOUSE_MASK(event->button.button);
        break;
    default:;
    }

    if (sdl_helper::sdl_key_to_vc(button) == m_keycode)
        m_pressed = pressed;
}

ElementButton* ElementButton::read_from_file(ccl_config* file, const std::string& id, SDL_Point* default_dim)
{
    return new ElementButton(id, read_position(file, id), read_mapping(file, id, default_dim),
        file->get_int(id + CFG_KEY_CODE), read_layer(file, id));
}
