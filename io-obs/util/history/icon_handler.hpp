/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "history_icons.hpp"
#include "handler.hpp"

class icon_handler : public handler
{
    history_icons m_icons;
public:
    icon_handler();

    ~icon_handler();

    void load_icons(const char* img, const char* cfg);

    void update() override;

    void tick(float seconds) override;

    void swap(input_entry* current) override;

    void render() override;

    void clear() override;
};
