/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */


#include "element_texture.hpp"
#include "../../ccl/ccl.hpp"

void element_texture::load(ccl_config* cfg, const std::string& id)
{
    read_pos(cfg, id);
    read_uv(cfg, id);
    read_size(cfg, id);
}

void element_texture::draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data)
{
    gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), image->texture);
    gs_matrix_push();
    gs_matrix_translate3f(m_pos.x, m_pos.y, 1.f);

    gs_draw_sprite_subregion(image->texture, 0, m_mapping.x, m_mapping.y,
        m_mapping.cx, m_mapping.cy);

    gs_matrix_pop();
}

void element_texture::draw(gs_effect_t* effect, gs_image_file_t* image, const gs_rect* rect) const
{
    gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), image->texture);
    gs_matrix_push();
    gs_matrix_translate3f(m_pos.x, m_pos.y, 1.f);
    gs_draw_sprite_subregion(image->texture, 0, rect->x, rect->y, rect->cx, rect->cy);
    gs_matrix_pop();
}
