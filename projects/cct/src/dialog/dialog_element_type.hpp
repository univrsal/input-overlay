/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2021 univrsal <uni@vrsal.de>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#pragma once

#include "../util/sdl_helper.hpp"
#include "dialog.hpp"

class sdl_helper;

class tool;

class combobox;

class DialogElementType : public dialog {
public:
    DialogElementType(sdl_helper *sdl, tool *tool);

    void init() override;

    void action_performed(int8_t action_id) override;

private:
    tool *m_tool;
    combobox *m_type = nullptr;
};
