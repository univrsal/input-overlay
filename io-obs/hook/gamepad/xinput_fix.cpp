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
#include "../../util/log.h"
#include <util/platform.h>
#include <util/bmem.h>
#include <layout_constants.h>
#include <string>
#include <Windows.h>

namespace xinput_fix {

typedef struct pad_handle {
    unsigned long eventCount;
    WORD wButtons;
    BYTE bLeftTrigger;
    BYTE bRightTrigger;
    SHORT sThumbLX;
    SHORT sThumbLY;
    SHORT sThumbRX;
    SHORT sThumbRY;
} pad_handle;

bool loaded = false;

/* Will be loaded from xinput1_3.dll */
int(__stdcall *get_gamepad_state)(int, pad_handle *);

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
    binfo("Loading xinput1_3.dll from %s", path_utf8.c_str());

	xinput_dll = LoadLibrary(dll_path.c_str());
	if (xinput_dll) {
        get_gamepad_state = reinterpret_cast<int(__stdcall *)(int, pad_handle *)>(GetProcAddress(xinput_dll, LPCSTR(100)));

		if (get_gamepad_state) {
            binfo("Loading of xinput succeeded");
			loaded = true;
		} else {
            binfo("Loading of xinput failed: %i", GetLastError());
		}
	}
}

void unload()
{
	if (loaded)
		FreeLibrary(xinput_dll);
}

bool pressed(pad_handle *pad, const gamepad::code c)
{
    return pad ? (pad->wButtons & c) != 0 : 0;
}

void get_dpad(pad_handle *pad, dpad_direction dirs[2])
{
	if (pad && dirs) {
		auto index = 0;
        if (pressed(pad, gamepad::CODE_DPAD_UP))
            dirs[index++] = DD_UP;
        if (pressed(pad, gamepad::CODE_DPAD_DOWN))
            dirs[index++] = DD_DOWN;
        if (pressed(pad, gamepad::CODE_DPAD_LEFT) && index < 2)
            dirs[index++] = DD_LEFT;
        if (pressed(pad, gamepad::CODE_DPAD_RIGHT) && index < 2)
            dirs[index++] = DD_RIGHT;
	}
}

pad_handle *create()
{
    return reinterpret_cast<pad_handle *>(bzalloc(sizeof(pad_handle)));
}

void free(pad_handle *pad)
{
    bfree(pad);
}

int update(pad_handle *pad, uint8_t id)
{
	if (pad && get_gamepad_state)
		return get_gamepad_state(id, pad);
	return -1;
}

float trigger_l(pad_handle *pad)
{
	if (pad)
		return pad->bLeftTrigger / TRIGGER_MAX_VAL;
	return 0.f;
}

float trigger_r(pad_handle *pad)
{
	if (pad)
		return pad->bRightTrigger / TRIGGER_MAX_VAL;
	return 0.f;
}

float stick_l_x(pad_handle *pad)
{
	if (pad)
		return pad->sThumbLX / STICK_MAX_VAL;
	return 0.f;
}

float stick_l_y(pad_handle *pad)
{
	if (pad)
		return pad->sThumbLY / STICK_MAX_VAL;
	return 0.f;
}

float stick_r_x(pad_handle *pad)
{
	if (pad)
		return pad->sThumbRX / STICK_MAX_VAL;
	return 0.f;
}

float stick_r_y(pad_handle *pad)
{
	if (pad)
		return pad->sThumbRY / STICK_MAX_VAL;
	return 0.f;
}

}
