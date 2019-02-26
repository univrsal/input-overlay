/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "icon_handler.hpp"

icon_handler::icon_handler()
{
}

icon_handler::~icon_handler()
{
}

void icon_handler::load_icons(const char* img, const char* cfg)
{
    m_icons.load_from_file(cfg, img);
}
