/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "../../sources/input_source.hpp"
#include "element_texture.hpp"
#include "../../../ccl/ccl.hpp"

void element_texture::load(ccl_config* cfg, const std::string& id)
{
    read_pos(cfg, id);
    read_mapping(cfg, id);
}

void element_texture::draw(gs_effect_t* effect, gs_image_file_t* image,
                           element_data* data, sources::shared_settings* settings)
{
    draw(effect, image, &m_mapping, &m_pos);
}

void element_texture::draw(gs_effect_t* effect, gs_image_file_t* image,
                           const gs_rect* rect) const
{
    draw(effect, image, rect ? rect : &m_mapping, &m_pos);
}

void element_texture::draw(gs_effect_t* effect, gs_image_file_t* image,
                           const gs_rect* rect, const vec2* pos)
{
    gs_matrix_push();
    gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"),
                          image->texture);
    gs_matrix_translate3f(pos->x, pos->y, 1.f);
    gs_draw_sprite_subregion(image->texture, 0, rect->x, rect->y, rect->cx,
                             rect->cy);
    gs_matrix_pop();
}
