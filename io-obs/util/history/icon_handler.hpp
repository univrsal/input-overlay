/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include "history_icons.hpp"

class icon_handler
{
    history_icons m_icons;
public:
    icon_handler();
    ~icon_handler();

    void load_icons(const char* img, const char* cfg);
};
