/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "../layout_constants.hpp"
#include "element_texture.hpp"

#define ID_PRESSED 3

class element_gamepad_id : public element_texture
{
public:
    element_gamepad_id() : element_texture(GAMEPAD_ID)
    {
    };

    void load(ccl_config* cfg, const std::string& id) override;

    void draw(gs_effect_t* effect, gs_image_file_t* image,
        element_data* data, sources::shared_settings* settings) override;

    data_source get_source() override { return GAMEPAD; }

private:
    /* 0 - 2 Player 2 - 4 (Player 1 is default)
     * 3     Middle pressed down
     */
    gs_rect m_mappings[4];
};
