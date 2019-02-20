/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "element.hpp"

class element_texture : public element
{
public:
    element_texture();

    element_texture(element_type type);

    void load(ccl_config* cfg, const std::string& id) override;

    void draw(gs_effect_t* effect, gs_image_file_t* image,
        element_data* data, sources::overlay_settings* settings) override;
    void draw(gs_effect_t* effect, gs_image_file_t* image,
        const gs_rect* rect) const;
    static void draw(gs_effect_t* effect, gs_image_file_t* image,
        const gs_rect* rect, const vec2* pos);
    static void draw(gs_effect* effect, gs_image_file_t* image,
        const gs_rect* rect, const vec2* pos, float angle);

    data_source get_source() override;
};
