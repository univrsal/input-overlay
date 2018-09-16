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
    element_texture() : element(TEXTURE)
    {
        /* NO-OP */
    };

    element_texture(const element_type type) : element(type)
    {
        /* NO-OP */
    }

    void load(ccl_config* cfg, const std::string& id,
              const vec2* default_size) override;
    void draw(gs_effect_t* effect, gs_image_file_t* image,
              element_data* data) override;
    void draw(gs_effect_t* effect, gs_image_file_t* image,
              const gs_rect* rect) const;
    static void draw(gs_effect_t* effect, gs_image_file_t* image,
                     const gs_rect* rect, const vec2* pos);

     data_source get_source() override { return DEFAULT; }
};
