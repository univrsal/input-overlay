/**
 * Created by universal on 11.09.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#include "../dialog/dialog_new_element.hpp"
#include "../util/coordinate_system.hpp"
#include "../util/texture.hpp"
#include "../util/palette.hpp"
#include "../../../ccl/ccl.hpp"
#include "element_dpad.hpp"

void ElementDPad::draw(texture *atlas, coordinate_system *cs, const bool selected, const bool alpha)
{
	get_abs_dim(cs);
	if (m_dir == DD_CENTER) {
		atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping,
		            (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);
	} else {
		auto temp = m_mapping;
		temp.x += (CFG_INNER_BORDER + temp.w) * m_dir;
		atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &temp,
		            (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);
	}

	if (selected)
		cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->get_palette()->red());
}

void ElementDPad::handle_event(SDL_Event *event, sdl_helper *helper)
{
	if (event->type == SDL_CONTROLLERBUTTONDOWN) {
		switch (event->cbutton.button) {
		case SDL_CONTROLLER_BUTTON_DPAD_UP:
			m_dir |= DD_UP;
			break;
		case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
			m_dir |= DD_DOWN;
			break;
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
			m_dir |= DD_LEFT;
			break;
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
			m_dir |= DD_RIGHT;
			break;
		default: ;
		}
	} else if (event->type == SDL_CONTROLLERBUTTONUP) {
		switch (event->cbutton.button) {
		case SDL_CONTROLLER_BUTTON_DPAD_UP:
			m_dir &= ~DD_UP;
			break;
		case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
			m_dir &= ~DD_DOWN;
			break;
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
			m_dir &= ~DD_LEFT;
			break;
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
			m_dir &= ~DD_RIGHT;
			break;
		default: ;
		}
	}
}

ElementDPad *ElementDPad::read_from_file(ccl_config *file, const std::string &id, SDL_Point *default_dim)
{
	return new ElementDPad(id, read_position(file, id), read_mapping(file, id, default_dim), read_layer(file, id));
}

void ElementDPad::write_to_file(ccl_config *cfg, SDL_Point *default_dim, uint8_t &layout_flags)
{
	element_texture::write_to_file(cfg, default_dim, layout_flags);
	layout_flags |= OF_GAMEPAD;
}
