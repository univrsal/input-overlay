/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "element_texture.hpp"

#define ID_PRESSED 3

class element_gamepad_id : public element_texture
{
public:
    element_gamepad_id();

    void load(ccl_config* cfg, const std::string& id) override;

    void draw(gs_effect_t* effect, gs_image_file_t* image,
        element_data* data, sources::overlay_settings* settings) override;

    data_source get_source() override;

private:
    /* 0 - 2 Player 2 - 4 (Player 1 is default)
     * 3     Middle pressed down
     */
    gs_rect m_mappings[4];
};
