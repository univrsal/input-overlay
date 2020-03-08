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

#include "../../sources/input_source.hpp"
#include "element_texture.hpp"
#include "../../../ccl/ccl.hpp"
#include "util/layout_constants.hpp"

extern "C" {
#include <graphics/image-file.h>
}

element_texture::element_texture() : element(ET_TEXTURE)
{
	/* NO-OP */
}

element_texture::element_texture(const element_type type) : element(type)
{
	/* NO-OP */
}

void element_texture::load(ccl_config *cfg, const std::string &id)
{
	read_pos(cfg, id);
	read_mapping(cfg, id);
}

void element_texture::draw(gs_effect_t *effect, gs_image_file_t *image, element_data *data,
						   sources::overlay_settings *settings)
{
	UNUSED_PARAMETER(data);
	UNUSED_PARAMETER(settings);
	draw(effect, image, &m_mapping, &m_pos);
}

void element_texture::draw(gs_effect_t *effect, gs_image_file_t *image, const gs_rect *rect) const
{
	draw(effect, image, rect ? rect : &m_mapping, &m_pos);
}

void element_texture::draw(gs_effect_t *effect, gs_image_file_t *image, const gs_rect *rect, const vec2 *pos)
{
	gs_matrix_push();
	gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), image->texture);
	gs_matrix_translate3f(pos->x, pos->y, 1.f);
	gs_draw_sprite_subregion(image->texture, 0, rect->x, rect->y, rect->cx, rect->cy);
	gs_matrix_pop();
}

void element_texture::draw(gs_effect *effect, gs_image_file_t *image, const gs_rect *rect, const vec2 *pos,
						   const float angle)
{
	gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), image->texture);

	gs_matrix_push();
	{
		/* Put into position */
		gs_matrix_translate3f(pos->x, pos->y + rect->cy, 1.f);

		/* Offset to rotation center (I think :P, rotations are tricky) */
		gs_matrix_translate3f(-(rect->cx / 2.f), -(rect->cy / 2.f), 1.f);
		gs_matrix_rotaa4f(0.f, 0.f, 1.f, angle);

		/* Offset back */
		gs_matrix_translate3f(-(rect->cx / 2.f), -(rect->cy / 2.f), 1.f);

		gs_draw_sprite_subregion(image->texture, 0, rect->x, rect->y, rect->cx, rect->cy);
	}
	gs_matrix_pop();
}

data_source element_texture::get_source()
{
	return DS_NONE;
}
