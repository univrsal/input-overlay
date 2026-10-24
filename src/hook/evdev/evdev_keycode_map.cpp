/*************************************************************************
* This file is part of input-overlay
 * git.vrsal.cc/alex/input-overlay
 * based on https://codeberg.org/MaxCross/obs-input-overlay-zig
 * Copyright 2026 univrsal <uni@vrsal.cc>.
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

#include "evdev_keycode_map.hpp"

#include <linux/input-event-codes.h>

uint16_t evdev_to_uiohook(uint16_t evdev_code)
{
    // Same mapping libuiohook uses for its Linux back-ends (src/linux/shared/input_helper.c),
    // so evdev reports the same key codes as libuiohook
    // clang-format off
    switch (evdev_code) {
    case KEY_ESC:              return VC_ESCAPE;
    case KEY_F1:               return VC_F1;
    case KEY_F2:               return VC_F2;
    case KEY_F3:               return VC_F3;
    case KEY_F4:               return VC_F4;
    case KEY_F5:               return VC_F5;
    case KEY_F6:               return VC_F6;
    case KEY_F7:               return VC_F7;
    case KEY_F8:               return VC_F8;
    case KEY_F9:               return VC_F9;
    case KEY_F10:              return VC_F10;
    case KEY_F11:              return VC_F11;
    case KEY_F12:              return VC_F12;
    case KEY_F13:              return VC_F13;
    case KEY_F14:              return VC_F14;
    case KEY_F15:              return VC_F15;
    case KEY_F16:              return VC_F16;
    case KEY_F17:              return VC_F17;
    case KEY_F18:              return VC_F18;
    case KEY_F19:              return VC_F19;
    case KEY_F20:              return VC_F20;
    case KEY_F21:              return VC_F21;
    case KEY_F22:              return VC_F22;
    case KEY_F23:              return VC_F23;
    case KEY_F24:              return VC_F24;
    case KEY_GRAVE:            return VC_BACK_QUOTE;
    case KEY_1:                return VC_1;
    case KEY_2:                return VC_2;
    case KEY_3:                return VC_3;
    case KEY_4:                return VC_4;
    case KEY_5:                return VC_5;
    case KEY_6:                return VC_6;
    case KEY_7:                return VC_7;
    case KEY_8:                return VC_8;
    case KEY_9:                return VC_9;
    case KEY_0:                return VC_0;
    case KEY_MINUS:            return VC_MINUS;
    case KEY_EQUAL:            return VC_EQUALS;
    case KEY_BACKSPACE:        return VC_BACKSPACE;
    case KEY_TAB:              return VC_TAB;
    case KEY_Q:                return VC_Q;
    case KEY_W:                return VC_W;
    case KEY_E:                return VC_E;
    case KEY_R:                return VC_R;
    case KEY_T:                return VC_T;
    case KEY_Y:                return VC_Y;
    case KEY_U:                return VC_U;
    case KEY_I:                return VC_I;
    case KEY_O:                return VC_O;
    case KEY_P:                return VC_P;
    case KEY_LEFTBRACE:        return VC_OPEN_BRACKET;
    case KEY_RIGHTBRACE:       return VC_CLOSE_BRACKET;
    case KEY_ENTER:            return VC_ENTER;
    case KEY_CAPSLOCK:         return VC_CAPS_LOCK;
    case KEY_A:                return VC_A;
    case KEY_S:                return VC_S;
    case KEY_D:                return VC_D;
    case KEY_F:                return VC_F;
    case KEY_G:                return VC_G;
    case KEY_H:                return VC_H;
    case KEY_J:                return VC_J;
    case KEY_K:                return VC_K;
    case KEY_L:                return VC_L;
    case KEY_SEMICOLON:        return VC_SEMICOLON;
    case KEY_APOSTROPHE:       return VC_QUOTE;
    case KEY_BACKSLASH:        return VC_BACK_SLASH;
    case KEY_LEFTSHIFT:        return VC_SHIFT_L;
    case KEY_Z:                return VC_Z;
    case KEY_X:                return VC_X;
    case KEY_C:                return VC_C;
    case KEY_V:                return VC_V;
    case KEY_B:                return VC_B;
    case KEY_N:                return VC_N;
    case KEY_M:                return VC_M;
    case KEY_COMMA:            return VC_COMMA;
    case KEY_DOT:              return VC_PERIOD;
    case KEY_SLASH:            return VC_SLASH;
    case KEY_RIGHTSHIFT:       return VC_SHIFT_R;
    case KEY_102ND:            return VC_SECTION;
    case KEY_LEFTALT:          return VC_ALT_L;
    case KEY_LEFTCTRL:         return VC_CONTROL_L;
    case KEY_LEFTMETA:         return VC_META_L;
    case KEY_SPACE:            return VC_SPACE;
    case KEY_RIGHTMETA:        return VC_META_R;
    case KEY_RIGHTCTRL:        return VC_CONTROL_R;
    case KEY_RIGHTALT:         return VC_ALT_R;
    case KEY_COMPOSE:          return VC_CONTEXT_MENU;
    case KEY_SYSRQ:            return VC_PRINT_SCREEN;
    case KEY_SCROLLLOCK:       return VC_SCROLL_LOCK;
    case KEY_PAUSE:            return VC_PAUSE;
    case KEY_INSERT:           return VC_INSERT;
    case KEY_HOME:             return VC_HOME;
    case KEY_PAGEUP:           return VC_PAGE_UP;
    case KEY_DELETE:           return VC_DELETE;
    case KEY_END:              return VC_END;
    case KEY_PAGEDOWN:         return VC_PAGE_DOWN;
    case KEY_UP:               return VC_UP;
    case KEY_LEFT:             return VC_LEFT;
    case KEY_DOWN:             return VC_DOWN;
    case KEY_RIGHT:            return VC_RIGHT;
    case KEY_NUMLOCK:          return VC_NUM_LOCK;
    case KEY_KPSLASH:          return VC_KP_DIVIDE;
    case KEY_KPASTERISK:       return VC_KP_MULTIPLY;
    case KEY_KPMINUS:          return VC_KP_SUBTRACT;
    case KEY_KP7:              return VC_KP_7;
    case KEY_KP8:              return VC_KP_8;
    case KEY_KP9:              return VC_KP_9;
    case KEY_KPPLUS:           return VC_KP_ADD;
    case KEY_KP4:              return VC_KP_4;
    case KEY_KP5:              return VC_KP_5;
    case KEY_KP6:              return VC_KP_6;
    case KEY_KP1:              return VC_KP_1;
    case KEY_KP2:              return VC_KP_2;
    case KEY_KP3:              return VC_KP_3;
    case KEY_KPENTER:          return VC_KP_ENTER;
    case KEY_KP0:              return VC_KP_0;
    case KEY_KPDOT:            return VC_KP_DECIMAL;
    case KEY_KPEQUAL:          return VC_KP_EQUALS;
    case KEY_KATAKANAHIRAGANA: return VC_KATAKANA_HIRAGANA;
    case KEY_RO:               return VC_UNDERSCORE;
    case KEY_HENKAN:           return VC_CONVERT;
    case KEY_MUHENKAN:         return VC_NONCONVERT;
    case KEY_YEN:              return VC_YEN;
    case KEY_KATAKANA:         return VC_KATAKANA;
    case KEY_HIRAGANA:         return VC_HIRAGANA;
    case KEY_KPJPCOMMA:        return VC_JP_COMMA;
    case KEY_HANGEUL:          return VC_KANA;
    case KEY_HANJA:            return VC_HANJA;
    case KEY_MUTE:             return VC_VOLUME_MUTE;
    case KEY_VOLUMEDOWN:       return VC_VOLUME_DOWN;
    case KEY_VOLUMEUP:         return VC_VOLUME_UP;
    case KEY_POWER:            return VC_POWER;
    case KEY_HELP:             return VC_HELP;
    case KEY_KPCOMMA:          return VC_KP_SEPARATOR;
    case KEY_CALC:             return VC_APP_CALCULATOR;
    case KEY_SLEEP:            return VC_SLEEP;
    case KEY_XFER:             return VC_MODE_CHANGE;
    case KEY_PROG1:            return VC_APP_1;
    case KEY_PROG2:            return VC_APP_2;
    case KEY_WWW:              return VC_APP_BROWSER;
    case KEY_MAIL:             return VC_APP_MAIL;
    case KEY_BOOKMARKS:        return VC_BROWSER_FAVORITES;
    case KEY_BACK:             return VC_BROWSER_BACK;
    case KEY_FORWARD:          return VC_BROWSER_FORWARD;
    case KEY_EJECTCD:          return VC_MEDIA_EJECT;
    case KEY_NEXTSONG:         return VC_MEDIA_NEXT;
    case KEY_PLAYPAUSE:        return VC_MEDIA_PLAY;
    case KEY_PREVIOUSSONG:     return VC_MEDIA_PREVIOUS;
    case KEY_STOPCD:           return VC_MEDIA_STOP;
    case KEY_HOMEPAGE:         return VC_BROWSER_HOME;
    case KEY_REFRESH:          return VC_BROWSER_REFRESH;
    case KEY_PROG3:            return VC_APP_3;
    case KEY_PROG4:            return VC_APP_4;
    case KEY_SEARCH:           return VC_BROWSER_SEARCH;
    case KEY_CANCEL:           return VC_CANCEL;
    case KEY_STOP:             return VC_BROWSER_STOP;
    case KEY_MEDIA:            return VC_MEDIA_SELECT;
    default:                   return VC_UNDEFINED;
    }
    // clang-format on
}

uint16_t evdev_mouse_to_uiohook(uint16_t evdev_code)
{
    switch (evdev_code) {
    case BTN_LEFT:
        return MOUSE_BUTTON1;
    case BTN_RIGHT:
        return MOUSE_BUTTON2;
    case BTN_MIDDLE:
        return MOUSE_BUTTON3;
    case BTN_SIDE:
        return MOUSE_BUTTON4;
    case BTN_EXTRA:
        return MOUSE_BUTTON5;
    default:
        return 0;
    }
}