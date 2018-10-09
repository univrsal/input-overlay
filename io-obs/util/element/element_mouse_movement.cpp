/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "../../sources/input_source.hpp"
#include "../../../ccl/ccl.hpp"
#include "element_mouse_movement.hpp"
#include "../util.hpp"

void element_mouse_movement::load(ccl_config* cfg, const std::string& id)
{
    element_texture::load(cfg, id);
}

void element_mouse_movement::draw(gs_effect_t* effect,
    gs_image_file_t* image, element_data* data, sources::shared_settings* settings)
{
}

void element_data_mouse_movement::merge(element_data* other)
{
}
