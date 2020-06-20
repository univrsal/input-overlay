/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
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
#include "../dialog/dialog_element_settings.hpp"
#include "../dialog/dialog_new_element.hpp"
#include "../util/coordinate_system.hpp"
#include "../util/notifier.hpp"
#include "../util/palette.hpp"
#include "../util/sdl_helper.hpp"
#include "../util/texture.hpp"
#include <util.hpp>
#include <keycodes.h>

element_button::element_button(const std::string &id, const SDL_Point pos, const SDL_Rect mapping, const uint16_t vc,
							   const uint8_t z)
	: element_texture(ET_BUTTON, id, pos, mapping, z)
{
	m_keycode = vc;
	m_pressed_mapping = m_mapping;
	m_pressed_mapping.y += m_mapping.h + CFG_INNER_BORDER;
}

element_error element_button::is_valid(notifier *n, sdl_helper *h)
{
	auto error = element_texture::is_valid(n, h);
	if (error == EE_VALID && m_keycode == 0x0) {
		n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_KEYCODE_INVALID));
		error = EE_KEYCODE_INVALID;
	}
	return error;
}

void element_button::draw(texture *atlas, coordinate_system *cs, const bool selected, const bool alpha)
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

void element_button::write_to_json(json_obj &j, SDL_Point *default_dim, uint8_t &layout_flags)
{
	element_texture::write_to_json(j, default_dim, layout_flags);
	j[CFG_KEY_CODE] = m_keycode;

	if ((m_keycode >> 8) == (VC_PAD_MASK >> 8))
		layout_flags |= OF_GAMEPAD;
}

void element_button::update_settings(dialog_new_element *dialog)
{
	element_texture::update_settings(dialog);
	m_pressed_mapping = m_mapping;
	m_pressed_mapping.y += m_mapping.h + CFG_INNER_BORDER;
	m_keycode = dialog->get_vc();
}

void element_button::update_settings(dialog_element_settings *dialog)
{
	element_texture::update_settings(dialog);
	m_pressed_mapping = m_mapping;
	m_pressed_mapping.y += m_mapping.h + CFG_INNER_BORDER;
	m_keycode = dialog->get_vc();
}

void element_button::handle_event(SDL_Event *event, sdl_helper *helper)
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

element_button *element_button::read_from_json(const json &j, SDL_Point *default_dim)
{
	return new element_button(j[CFG_ID].string_value(), read_position(j), read_mapping(j, default_dim),
							  j[CFG_KEY_CODE].number_value(), j[CFG_Z_LEVEL].number_value());
}
