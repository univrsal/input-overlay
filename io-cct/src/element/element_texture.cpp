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

#include "element_texture.hpp"
#include "../../../ccl/ccl.hpp"
#include "../dialog/dialog_element_settings.hpp"
#include "../dialog/dialog_new_element.hpp"
#include "../util/coordinate_system.hpp"
#include "../util/notifier.hpp"
#include "../util/palette.hpp"
#include "../util/texture.hpp"
#include <utility>

element_texture::element_texture(const std::string &id, const SDL_Point pos, const SDL_Rect mapping, const uint8_t z)
	: element(ET_TEXTURE, id, pos, z)
{
	m_mapping = mapping;
}

element_texture::element_texture(const element_type t, const std::string &id, const SDL_Point pos,
								 const SDL_Rect mapping, const uint8_t z)
	: element(t, id, pos, z)
{
	m_mapping = mapping;
}

element_error element_texture::is_valid(notifier *n, sdl_helper *h)
{
	auto result = element::is_valid(n, h);
	if (result == EE_VALID && SDL_RectEmpty(&m_mapping)) {
		n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_SELECTION_EMTPY));
		result = EE_MAPPING_EMPTY;
	}
	return result;
}

void element_texture::draw(texture *atlas, coordinate_system *cs, const bool selected, const bool alpha)
{
	get_abs_dim(cs);
	atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping,
				(alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);

	if (selected)
		cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->get_palette()->red());
}

void element_texture::write_to_file(ccl_config *cfg, SDL_Point *default_dim, uint8_t &layout_flags)
{
	element::write_to_file(cfg, default_dim, layout_flags);
	const auto comment = "Mapping of " + m_id;
	cfg->add_rect(m_id + CFG_MAPPING, comment, m_mapping.x, m_mapping.y, m_mapping.w, m_mapping.h);
}

void element_texture::update_settings(dialog_new_element *dialog)
{
	element::update_settings(dialog);
	set_mapping(dialog->get_selection());
}

void element_texture::update_settings(dialog_element_settings *dialog)
{
	element::update_settings(dialog);
	set_mapping(dialog->get_mapping());
}

element_texture *element_texture::read_from_file(ccl_config *file, const std::string &id, SDL_Point *default_dim)
{
	return new element_texture(id, read_position(file, id), read_mapping(file, id, default_dim), read_layer(file, id));
}
