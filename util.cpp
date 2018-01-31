#include "util.hpp"

/**
 * This file is part of input-overlay
 * which is licenced under the MIT licence.
 * See LICENCE or https://mit-license.org
 * github.com/univrsal/input-overlay
 */

/**
 * Not the smartest way but I didn't
 * want to bother with the windows
 * API anymore
 */
const char * key_to_text(int key_code)
{
    switch (key_code)
    {
    case VC_KP_0:
        return "NUMPAD 0";
    case VC_KP_1:
        return "NUMPAD 1";
    case VC_KP_2:
        return "NUMPAD 2";
    case VC_KP_3:
        return "NUMPAD 3";
    case VC_KP_4:
        return "NUMPAD 4";
    case VC_KP_5:
        return "NUMPAD 5";
    case VC_KP_6:
        return "NUMPAD 6";
    case VC_KP_7:
        return "NUMPAD 7";
    case VC_KP_8:
        return "NUMPAD 8";
    case VC_KP_9:
        return "NUMPAD 9";  
    case VC_NUM_LOCK:
        return "NUM LOCK";
    case VC_KP_MULTIPLY:
        return "MULTIPLY";
    case VC_KP_ADD:
        return "ADD";
    case VC_KP_SUBTRACT:
        return "SUBTRACT";
    case VC_KP_COMMA:
        return "DECIMAL";
    case VC_KP_DIVIDE:
        return "DIVIDE";
    case VC_F1:
        return "F1";
    case VC_F2:
        return "F2";
    case VC_F3:
        return "F3";
    case VC_F4:
        return "F4";
    case VC_F5:
        return "F5";
    case VC_F6:
        return "F6";
    case VC_F7:
        return "F7";
    case VC_F8:
        return "F8";
    case VC_F9:
        return "F9";
    case VC_F10:
        return "F10";
    case VC_F11:
        return "F11";
    case VC_F12:
        return "F12";
    case VC_F13:
        return "F13";
    case VC_F14:
        return "F14";
    case VC_F15:
        return "F15";
    case VC_F16:
        return "F16";
    case VC_F17:
        return "F17";
    case VC_F18:
        return "F18";
    case VC_F19:
        return "F19";
    case VC_F20:
        return "F20";
    case VC_F21:
        return "F21";
    case VC_F22:
        return "F22";
    case VC_F23:
        return "F23";
    case VC_F24:
        return "F24";
    case VC_A:
        return "A";
    case VC_B:
        return "B";
    case VC_C:
        return "C";
    case VC_D:
        return "D";
    case VC_E:
        return "E";
    case VC_F:
        return "F";
    case VC_G:
        return "G";
    case VC_H:
        return "H";
    case VC_I:
        return "I";
    case VC_J:
        return "J";
    case VC_K:
        return "K";
    case VC_L:
        return "L";
    case VC_M:
        return "M";
    case VC_N:
        return "N";
    case VC_O:
        return "O";
    case VC_P:
        return "P";
    case VC_Q:
        return "Q";
    case VC_R:
        return "R";
    case VC_S:
        return "S";
    case VC_T:
        return "T";
    case VC_U:
        return "U";
    case VC_V:
        return "V";
    case VC_W:
        return "W";
    case VC_X:
        return "X";
    case VC_Y:
        return "Y";
    case VC_Z:
        return "Z";
    case VC_0:
        return "0";
    case VC_1:
        return "1";
    case VC_2:
        return "2";
    case VC_3:
        return "3";
    case VC_4:
        return "4";
    case VC_5:
        return "5";
    case VC_6:
        return "6";
    case VC_7:
        return "7";
    case VC_8:
        return "8";
    case VC_9:
        return "9";
    case VC_SHIFT_L:
        return "L-SHIFT";
    case VC_SHIFT_R:
        return "R-SHIFT";
    case VC_CONTROL_L:
        return "L-CONTROL";
    case VC_CONTROL_R:
        return "R-CONTROL";
    case VC_ALT_L:
        return "L-ALT";
    case VC_ALT_R:
        return "R-ALT";
    case VC_META_L:
        return "L-WIN";
    case VC_META_R:
        return "R-WIN";
    case VC_ENTER:
    case VC_KP_ENTER:
        return "ENTER";
    case VC_SPACE:
        return "SPACE";
    case VC_TAB:
        return "TAB";
    case VC_BACKSPACE:
        return "BACKSPACE";
    case VC_ESCAPE:
        return "ESC";
    case VC_INSERT:
        return "INSERT";
    case VC_HOME:
        return "HOME";
    case VC_PAGE_UP:
        return "PAGE UP";
    case VC_PAGE_DOWN:
        return "PAGE DOWN";
    case VC_END:
        return "END";
    case VC_DELETE:
        return "DELETE";
    case VC_UP:
    case VC_KP_UP:
        return "UP";
    case VC_DOWN:
    case VC_KP_DOWN:
        return "DOWN";
    case VC_LEFT:
    case VC_KP_LEFT:
        return "LEFT";
    case VC_RIGHT:
    case VC_KP_RIGHT:
        return "RIGHT";
    case VC_PRINTSCREEN:
        return "PRINT";
    case VC_SCROLL_LOCK:
        return "SCROLL LOCK";
    case VC_PAUSE:
        return "PAUSE";
    case VC_CAPS_LOCK:
        return "CAPSLOCK";
    case VC_MOUSE_BUTTON1:
        return "LEFT MOUSE";
    case VC_MOUSE_BUTTON2:
        return "RIGHT MOUSE";
    case VC_MOUSE_BUTTON3:
        return "MIDDLE MOUSE";
    case VC_MOUSE_BUTTON4: // If you have a better name for them
        return "MOUSE4";   // lemme know
    case VC_MOUSE_BUTTON5:
        return "MOUSE5";
    case VC_MOUSE_WHEEL_UP:
        return "SCROLL UP";
    case VC_MOUSE_WHEEL_DOWN:
        return "SCROLL DOWN";
    default:
        return NULL;
        break;
    }
}

float get_angle(int16_t x, int16_t y)
{
    int quad = 0;
    if (x > 0)
    {
        if (y > 0)
            quad = 0;
        else
            quad = 3;
    }
    else
    {
        if (y < 0)
            quad = 2;
        else
            quad = 1;
    }

    return atan2f(((float) abs(x)), (float) abs(y)) * 180 / PI + (quad * 90.f);
}

uint16_t util_read_int(std::string & l)
{
    std::string temp = l.substr(0, l.find(','));
    l = l.substr(l.find(',') + 1, l.size());
    return std::atoi(temp.c_str());
}

uint16_t util_read_hex(std::string & l)
{
    std::string temp = l.substr(0, l.find(','));
    l = l.substr(l.find(',') + 1, l.size());

    if (temp.find("0x") != std::string::npos)
    {
        return std::stoul(temp, nullptr, 16);
    }
    else
    {
        return (uint16_t)0x0;
    }
}

std::string util_file_filter(const char* display, const char * formats)
{
    std::string filter = display;
    filter += " (";
    filter += formats;
    filter += ");;";
    filter += T_FILTER_ALL_FILES;
    filter += " (*.*)";

    return filter;
}

void util_format_path(std::string & path)
{
    std::replace(path.begin(), path.end(), '\\', '/');
    const char * slash = strrchr(path.c_str(), '/');
    if (slash)
        path.resize(slash - path.c_str() + 1);
}
