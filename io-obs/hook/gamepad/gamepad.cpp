/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <universailp@web.de>.
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

#include "gamepad.hpp"
#include <keycodes.h>

namespace gamepad {

    // clang-format off
    code all_codes[13] = { CODE_A, CODE_B, CODE_X, CODE_Y, CODE_GUIDE,
                           CODE_DPAD_DOWN, CODE_DPAD_UP, CODE_DPAD_LEFT,
                           CODE_DPAD_RIGHT, CODE_LEFT_SHOULDER,
                           CODE_RIGHT_SHOULDER, CODE_START, CODE_BACK };
    // clang-format on

    uint16_t to_vc(const code code)
    {
        switch (code) {
        case CODE_A:
            return VC_PAD_A;
        case CODE_B:
            return VC_PAD_B;
        case CODE_X:
            return VC_PAD_X;
        case CODE_Y:
            return VC_PAD_Y;
        case CODE_DPAD_DOWN:
            return VC_PAD_DPAD_DOWN;
        case CODE_DPAD_UP:
            return VC_PAD_DPAD_UP;
        case CODE_DPAD_LEFT:
            return VC_PAD_DPAD_LEFT;
        case CODE_DPAD_RIGHT:
            return VC_PAD_DPAD_RIGHT;
        case CODE_GUIDE:
            return VC_PAD_GUIDE;
        case CODE_LEFT_SHOULDER:
            return VC_PAD_LB;
        case CODE_RIGHT_SHOULDER:
            return VC_PAD_RB;
        case CODE_START:
            return VC_PAD_START;
        case CODE_BACK:
            return VC_PAD_BACK;
        default:
            return VC_NONE;
        }
    }

    handle::handle(int8_t id)
        : m_id(id)
    {
    }

    handle::~handle()
    {
        unload();
    }

    void handle::unload()
    {
        /* NO-OP */
    }

    void handle::update()
    {
        /* NO-OP */
    }

    bool handle::valid() const
    {
        return m_valid;
    }

    int8_t handle::id() const
    {
        return m_id;
    }
}
