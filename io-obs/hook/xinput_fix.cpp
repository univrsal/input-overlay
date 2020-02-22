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

#include "xinput_fix.hpp"
#include "util/util.hpp"
#include <keycodes.h>
#include <util/platform.h>

namespace xinput_fix {
gamepad_codes all_codes[13] = { CODE_A,
    CODE_B,
    CODE_X,
    CODE_Y,
    CODE_GUIDE,
    CODE_DPAD_DOWN,
    CODE_DPAD_UP,
    CODE_DPAD_LEFT,
    CODE_DPAD_RIGHT,
    CODE_LEFT_SHOULDER,
    CODE_RIGHT_SHOULDER,
    CODE_START,
    CODE_BACK };

uint16_t to_vc(const gamepad_codes code)
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

#ifdef _WIN32
bool loaded = false;

/* Will be loaded from xinput1_3.dll */
int(__stdcall* get_gamepad_state)(int, gamepad*);

TCHAR xinput_dll_path[MAX_PATH];
HINSTANCE xinput_dll;

void load()
{
    if (loaded)
        return;

    GetSystemDirectory(xinput_dll_path, sizeof(xinput_dll_path));
    std::wstring dll_path = L"\\xinput1_3.dll";
    dll_path = xinput_dll_path + dll_path;

    /* For logging */
    const auto len = os_wcs_to_utf8(dll_path.c_str(), 0, nullptr, 0);
    std::string path_utf8;
    path_utf8.resize(len);
    os_wcs_to_utf8(dll_path.c_str(), 0, &path_utf8[0], len + 1);
    blog(LOG_INFO, "Loading xinput1_3.dll from %s", path_utf8.c_str());

    xinput_dll = LoadLibrary(dll_path.c_str());
    if (xinput_dll) {
        get_gamepad_state = reinterpret_cast<int(__stdcall*)(int, gamepad*)>(GetProcAddress(xinput_dll, LPCSTR(100)));

        if (get_gamepad_state) {
            blog(LOG_INFO, "Loading of xinput succeeded");
            loaded = true;
        } else {
            blog(LOG_INFO, "Loading of xinput failed: %i", GetLastError());
        }
    }
}

void unload()
{
    if (loaded)
        FreeLibrary(xinput_dll);
}

bool pressed(gamepad* pad, const gamepad_codes code)
{
    return pad ? (pad->wButtons & code) != 0 : 0;
}

void get_dpad(gamepad* pad, dpad_direction dirs[2])
{
    if (pad && dirs) {
        auto index = 0;
        if (pressed(pad, CODE_DPAD_UP))
            dirs[index++] = dpad_direction::UP;
        if (pressed(pad, CODE_DPAD_DOWN))
            dirs[index++] = dpad_direction::DOWN;
        if (pressed(pad, CODE_DPAD_LEFT) && index < 2)
            dirs[index++] = dpad_direction::LEFT;
        if (pressed(pad, CODE_DPAD_RIGHT) && index < 2)
            dirs[index++] = dpad_direction::RIGHT;
    }
}

void free(gamepad* pad)
{
    RtlZeroMemory(pad, sizeof(gamepad));
}

int update(const int id, gamepad* pad)
{
    if (pad && get_gamepad_state)
        return get_gamepad_state(id, pad);
    return -1;
}

float trigger_l(gamepad* pad)
{
    if (pad)
        return pad->bLeftTrigger / TRIGGER_MAX_VAL;
    return 0.f;
}

float trigger_r(gamepad* pad)
{
    if (pad)
        return pad->bRightTrigger / TRIGGER_MAX_VAL;
    return 0.f;
}

float stick_l_x(gamepad* pad)
{
    if (pad)
        return pad->sThumbLX / STICK_MAX_VAL;
    return 0.f;
}

float stick_l_y(gamepad* pad)
{
    if (pad)
        return pad->sThumbLY / STICK_MAX_VAL;
    return 0.f;
}

float stick_r_x(gamepad* pad)
{
    if (pad)
        return pad->sThumbRX / STICK_MAX_VAL;
    return 0.f;
}

float stick_r_y(gamepad* pad)
{
    if (pad)
        return pad->sThumbRY / STICK_MAX_VAL;
    return 0.f;
}
#endif /* _WIN32 */

}
