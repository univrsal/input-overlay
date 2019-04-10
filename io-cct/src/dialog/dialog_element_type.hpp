/**
 * Created by univrsal on 15.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "dialog.hpp"
#include "../util/sdl_helper.hpp"

class sdl_helper;

class tool;

class combobox;

class DialogElementType : public dialog
{
public:
    DialogElementType(sdl_helper* sdl, tool* tool);

    void init() override;

    void action_performed(int8_t action_id) override;

private:
    tool* m_tool;
    combobox* m_type = nullptr;
};
