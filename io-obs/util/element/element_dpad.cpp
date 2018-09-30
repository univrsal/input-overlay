/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "../../sources/input_source.hpp"
#include "../../ccl/ccl.hpp"
#include "element_dpad.hpp"
#include "../util.hpp"

void element_dpad::load(ccl_config* cfg, const std::string& id)
{

}

void element_dpad::draw(gs_effect_t* effect,
    gs_image_file_t* image, element_data* data, sources::shared_settings* settings)
{
    
}

void element_data_dpad::merge(element_data* other)
{
    if (other && other->get_type() == m_type)
    {

    }
}
