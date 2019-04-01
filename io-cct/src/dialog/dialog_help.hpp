/**
 * Created by universal on 30.05.2018.
 * This file is part of reloded which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "dialog.hpp"
#include "../util/sdl_helper.hpp"
#include "../util/constants.hpp"
#include "elements/button.hpp"
#include "../tool.hpp"

class sdl_helper;

class tool;

class DialogHelp : public dialog
{
public:
    DialogHelp(sdl_helper* sdl, tool* tool) : dialog(sdl, SDL_Point{380, 485}, LANG_DIALOG_HELP)
    {
        m_tool = tool;
    };

    void init() override;

    void action_performed(int8_t action_id) override;

private:
    tool* m_tool = nullptr;
};
