/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */


#include "xinput_fix.hpp"
#include "util/util.hpp"
#include <util/platform.h>

namespace xinput_fix
{
    uint16_t to_vc(const gamepad_codes code)
    {
        switch (code)
        {
        case CODE_A: return PAD_TO_VC(PAD_A);
        case CODE_B: return PAD_TO_VC(PAD_B);
        case CODE_X: return PAD_TO_VC(PAD_X);
        case CODE_Y: return PAD_TO_VC(PAD_Y);
        case CODE_DPAD_DOWN: return PAD_TO_VC(PAD_DPAD_DOWN);
        case CODE_DPAD_UP: return PAD_TO_VC(PAD_DPAD_UP);
        case CODE_DPAD_LEFT: return PAD_TO_VC(PAD_DPAD_LEFT);
        case CODE_DPAD_RIGHT: return PAD_TO_VC(PAD_DPAD_RIGHT);
        case CODE_GUIDE: return PAD_TO_VC(PAD_X_BOX_KEY);
        case CODE_LEFT_SHOULDER: return PAD_TO_VC(PAD_LB);
        case CODE_RIGHT_SHOULDER: return PAD_TO_VC(PAD_RB);
        case CODE_START: return PAD_TO_VC(PAD_START);
        case CODE_BACK: return PAD_TO_VC(PAD_BACK);
        default: return 0xFFFF;
        }
    }

#ifdef _WIN32
    bool loaded = false;

    /* Will be loaded from xinput1_3.dll */
    int (__stdcall *get_gamepad_state)(int, gamepad*);

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
        if (xinput_dll)
        {
            get_gamepad_state = reinterpret_cast<int(__stdcall*)(int, gamepad*)>
                (GetProcAddress(xinput_dll, LPCSTR(100)));

            if (get_gamepad_state)
            {
                blog(LOG_INFO, "Loading of xinput succeeded");
                loaded = true;
            }
            else
            {
                blog(LOG_INFO, "Loading of xinput failed: %i", GetLastError());
            }
        }
    }

    void unload()
    {
        if (loaded)
            FreeLibrary(xinput_dll);
    }

    button_state pressed(gamepad* pad, const gamepad_codes code)
    {
        return pad ? button_state((pad->wButtons & code) != 0) : STATE_RELEASED;
    }

    void get_dpad(gamepad* pad, dpad_direction dirs[2])
    {
        if (pad && dirs)
        {
            auto index = 0;
            if (pressed(pad, CODE_DPAD_UP))
                dirs[index++] = DPAD_UP;
            if (pressed(pad, CODE_DPAD_DOWN))
                dirs[index++] = DPAD_DOWN;
            if (pressed(pad, CODE_DPAD_LEFT) && index < 2)
                dirs[index++] = DPAD_LEFT;
            if (pressed(pad, CODE_DPAD_RIGHT) && index < 2)
                dirs[index++] = DPAD_RIGHT;
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