#include "util.hpp"
#include <uiohook.h>
#include <algorithm>

/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

const char* key_to_text(const int key_code)
{
    switch (key_code) {
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
        case VC_MOUSE_BUTTON4:
            /* If you have a better name for them lemme know */
            return "MOUSE4";
        case VC_MOUSE_BUTTON5:
            return "MOUSE5";
        case VC_MOUSE_WHEEL_UP:
            return "SCROLL UP";
        case VC_MOUSE_WHEEL_DOWN:
            return "SCROLL DOWN";
        case VC_PAD_A:
            return "A";
        case VC_PAD_B:
            return "B";
        case VC_PAD_X:
            return "X";
        case VC_PAD_Y:
            return "Y";
        case VC_PAD_LB:
            return "LB";
        case VC_PAD_RB:
            return "RB";
        case VC_PAD_BACK:
            return "BACK";
        case VC_PAD_START:
            return "START";
        case VC_PAD_GUIDE:
            return "X-Box Button";
        case VC_PAD_L_ANALOG:
            return "Left Stick";
        case VC_PAD_R_ANALOG:
            return "Right Stick";
        case VC_PAD_DPAD_LEFT:
            return "DPad Left";
        case VC_PAD_DPAD_RIGHT:
            return "DPad Right";
        case VC_PAD_DPAD_UP:
            return "DPad Up";
        case VC_PAD_DPAD_DOWN:
            return "DPad Down";
        case VC_PAD_LT:
            return "LT";
        case VC_PAD_RT:
            return "RT";
        case VC_APP_MAIL:
            return "Mail app";
        case VC_APP_MUSIC:
            return "Music app";
        case VC_APP_CALCULATOR:
            return "Calculator app";
        case VC_APP_PICTURES:
            return "Picture app";
        case VC_MEDIA_PLAY:
            return "Play";
        case VC_MEDIA_NEXT:
            return "Next song";
        case VC_MEDIA_PREVIOUS:
            return "Previous song";
        case VC_MEDIA_EJECT:
            return "Eject media";
        case VC_MEDIA_SELECT:
            return "Select media";
        case VC_MEDIA_STOP:
            return "Stop media";
        case VC_VOLUME_DOWN:
            return "Volume down";
        case VC_VOLUME_UP:
            return "Volume up";
        case VC_VOLUME_MUTE:
            return "Volume mute/unmute";
        case VC_BROWSER_BACK:
            return "Browse back";
        case VC_BROWSER_FAVORITES:
            return "Browser favorites";
        case VC_BROWSER_FORWARD:
            return "Browse forward";
        case VC_BROWSER_HOME:
            return "Browser homepage";
        case VC_BROWSER_REFRESH:
            return "Browser refresh";
        case VC_BROWSER_STOP:
            return "Browser stop loading";
        case VC_CONTEXT_MENU:
            return "Open contextmenu";
        case VC_KATAKANA:
            return "Katakana";
        case VC_UNDERSCORE:
            return "_";
        case VC_FURIGANA:
            return "Furigana";
        case VC_KANJI:
            return "Kanji";
        case VC_HIRAGANA:
            return "Hiragana";
        case VC_YEN:
            return "Yen";
        case VC_SUN_HELP: /* wtf? */
            return "Sun help";
        case VC_SUN_STOP:
            return "Sun stop";
        case VC_SUN_PROPS:
            return "Sun properties";
        case VC_SUN_FRONT:
            return "Sun front";
        case VC_SUN_OPEN:
            return "Sun open";
        case VC_SUN_FIND:
            return "Sun find";
        case VC_SUN_AGAIN:
            return "Sun again";
        case VC_SUN_UNDO:
            return "Sun undo";
        case VC_SUN_COPY:
            return "Sun copy";
        case VC_SUN_INSERT:
            return "Sun insert";
        case VC_SUN_CUT:
            return "Sun cut";
        case VC_POWER:
            return "Power";
        case VC_SLEEP:
            return "Sleep";
        case VC_WAKE:
            return "Wake";
        case VC_CLEAR:
            return "Clear";
        case VC_BACKQUOTE:
            return "Backquote";
        case VC_MINUS:
            return "-";
        case VC_EQUALS:
            return "=";
        case VC_OPEN_BRACKET:
            return "[";
        case VC_CLOSE_BRACKET:
            return "]";
        case VC_BACK_SLASH:
            return "\\";
        case VC_SEMICOLON:
            return ";";
        case VC_QUOTE:
            return "\"";
        case VC_COMMA:
            return ",";
        case VC_PERIOD:
            return ".";
        case VC_SLASH:
            return "/";
        case VC_KP_PAGE_DOWN:
            return "Keypad page down";
        case VC_KP_PAGE_UP:
            return "Keypad page up";
        case VC_KP_END:
            return "Keypad end";
        case VC_KP_HOME:
            return "Keypad home";
        case VC_KP_INSERT:
            return "Keypad insert";
        case VC_KP_DELETE:
            return "Keypad delete";
        case VC_KP_SEPARATOR:
            return "Separator";
        default:
            return nullptr;
    }
}

std::string util_file_filter(const char* display, const char* formats)
{
    std::string filter = display;
    filter += " (";
    filter += formats;
    filter += ");;";
    filter += T_FILTER_ALL_FILES;
    filter += " (*.*)";

    return filter;
}

void util_format_path(std::string &path)
{
    std::replace(path.begin(), path.end(), '\\', '/');
    const auto slash = strrchr(path.c_str(), '/');
    if (slash)
        path.resize(slash - path.c_str() + 1);
}

void util_enable_mask(uint16_t &masks, const uint16_t mask)
{
    masks |= mask;
}

void util_disable_mask(uint16_t &masks, const uint16_t mask)
{
    masks &= ~mask;
}

void util_set_mask(uint16_t &masks, const uint16_t mask, const bool state)
{
    if (state) {
        util_enable_mask(masks, mask);
    } else {
        util_disable_mask(masks, mask);
    }
}

uint16_t util_mouse_to_vc(int m)
{
#ifndef _WIN32 /* Linux mixes right mouse and middle mouse or is windows getting it wrong? */
    if (m == 3)
        m = 2;
    else if (m == 2)
        m = 3;
#endif
    return static_cast<uint16_t>(VC_MOUSE_MASK | m);
}
