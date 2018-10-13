/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "../../sources/input_source.hpp"
#include "../../../ccl/ccl.hpp"
#include "element_text.hpp"
#include "util/layout_constants.hpp"

void element_text::load(ccl_config* cfg, const std::string& id)
{
}

void element_text::draw(gs_effect_t* effect,
    gs_image_file_t* image, element_data* data, sources::shared_settings* settings)
{
}

data_source element_text::get_source()
{
    return DEFAULT;
}

element_data_text::element_data_text()
    : element_data(TRIGGER)
{
}

bool element_data_text::is_persistent()
{
    return true;
}

void element_data_text::merge(element_data* other)
{
}

element_text::element_text()
    : element(TEXT)
{
};
