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

#include <cstddef>
#include <linux/input-event-codes.h>

// clang-format off
static const uint16_t evdev_to_uiohook_table[] = {
    /* 0   KEY_RESERVED       */ VC_UNDEFINED,
    /* 1   KEY_ESC            */ VC_ESCAPE,
    /* 2   KEY_1              */ VC_1,
    /* 3   KEY_2              */ VC_2,
    /* 4   KEY_3              */ VC_3,
    /* 5   KEY_4              */ VC_4,
    /* 6   KEY_5              */ VC_5,
    /* 7   KEY_6              */ VC_6,
    /* 8   KEY_7              */ VC_7,
    /* 9   KEY_8              */ VC_8,
    /* 10  KEY_9              */ VC_9,
    /* 11  KEY_0              */ VC_0,
    /* 12  KEY_MINUS          */ VC_MINUS,
    /* 13  KEY_EQUAL          */ VC_EQUALS,
    /* 14  KEY_BACKSPACE      */ VC_BACKSPACE,
    /* 15  KEY_TAB            */ VC_TAB,
    /* 16  KEY_Q              */ VC_Q,
    /* 17  KEY_W              */ VC_W,
    /* 18  KEY_E              */ VC_E,
    /* 19  KEY_R              */ VC_R,
    /* 20  KEY_T              */ VC_T,
    /* 21  KEY_Y              */ VC_Y,
    /* 22  KEY_U              */ VC_U,
    /* 23  KEY_I              */ VC_I,
    /* 24  KEY_O              */ VC_O,
    /* 25  KEY_P              */ VC_P,
    /* 26  KEY_LEFTBRACE      */ VC_OPEN_BRACKET,
    /* 27  KEY_RIGHTBRACE     */ VC_CLOSE_BRACKET,
    /* 28  KEY_ENTER          */ VC_ENTER,
    /* 29  KEY_LEFTCTRL       */ VC_CONTROL_L,
    /* 30  KEY_A              */ VC_A,
    /* 31  KEY_S              */ VC_S,
    /* 32  KEY_D              */ VC_D,
    /* 33  KEY_F              */ VC_F,
    /* 34  KEY_G              */ VC_G,
    /* 35  KEY_H              */ VC_H,
    /* 36  KEY_J              */ VC_J,
    /* 37  KEY_K              */ VC_K,
    /* 38  KEY_L              */ VC_L,
    /* 39  KEY_SEMICOLON      */ VC_SEMICOLON,
    /* 40  KEY_APOSTROPHE     */ VC_QUOTE,
    /* 41  KEY_GRAVE          */ VC_BACK_QUOTE,
    /* 42  KEY_LEFTSHIFT      */ VC_SHIFT_L,
    /* 43  KEY_BACKSLASH      */ VC_BACK_SLASH,
    /* 44  KEY_Z              */ VC_Z,
    /* 45  KEY_X              */ VC_X,
    /* 46  KEY_C              */ VC_C,
    /* 47  KEY_V              */ VC_V,
    /* 48  KEY_B              */ VC_B,
    /* 49  KEY_N              */ VC_N,
    /* 50  KEY_M              */ VC_M,
    /* 51  KEY_COMMA          */ VC_COMMA,
    /* 52  KEY_DOT            */ VC_PERIOD,
    /* 53  KEY_SLASH          */ VC_SLASH,
    /* 54  KEY_RIGHTSHIFT     */ VC_SHIFT_R,
    /* 55  KEY_KPASTERISK     */ VC_KP_MULTIPLY,
    /* 56  KEY_LEFTALT        */ VC_ALT_L,
    /* 57  KEY_SPACE          */ VC_SPACE,
    /* 58  KEY_CAPSLOCK       */ VC_CAPS_LOCK,
    /* 59  KEY_F1             */ VC_F1,
    /* 60  KEY_F2             */ VC_F2,
    /* 61  KEY_F3             */ VC_F3,
    /* 62  KEY_F4             */ VC_F4,
    /* 63  KEY_F5             */ VC_F5,
    /* 64  KEY_F6             */ VC_F6,
    /* 65  KEY_F7             */ VC_F7,
    /* 66  KEY_F8             */ VC_F8,
    /* 67  KEY_F9             */ VC_F9,
    /* 68  KEY_F10            */ VC_F10,
    /* 69  KEY_NUMLOCK        */ VC_NUM_LOCK,
    /* 70  KEY_SCROLLLOCK     */ VC_SCROLL_LOCK,
    /* 71  KEY_KP7            */ VC_KP_7,
    /* 72  KEY_KP8            */ VC_KP_8,
    /* 73  KEY_KP9            */ VC_KP_9,
    /* 74  KEY_KPMINUS        */ VC_KP_SUBTRACT,
    /* 75  KEY_KP4            */ VC_KP_4,
    /* 76  KEY_KP5            */ VC_KP_5,
    /* 77  KEY_KP6            */ VC_KP_6,
    /* 78  KEY_KPPLUS         */ VC_KP_ADD,
    /* 79  KEY_KP1            */ VC_KP_1,
    /* 80  KEY_KP2            */ VC_KP_2,
    /* 81  KEY_KP3            */ VC_KP_3,
    /* 82  KEY_KP0            */ VC_KP_0,
    /* 83  KEY_KPDOT          */ VC_KP_SEPARATOR,
    /* 84  (unused)           */ VC_UNDEFINED,
    /* 85  KEY_ZENKAKUHANKAKU */ VC_UNDEFINED,
    /* 86  KEY_102ND          */ VC_102,
    /* 87  KEY_F11            */ VC_F11,
    /* 88  KEY_F12            */ VC_F12,
    /* 89  KEY_RO             */ VC_UNDERSCORE,
    /* 90  KEY_KATAKANA       */ VC_KATAKANA,
    /* 91  KEY_HIRAGANA       */ VC_HIRAGANA,
    /* 92  KEY_HENKAN         */ VC_KANJI,
    /* 93  KEY_KATAKANAHIRAGANA */ VC_KATAKANA_HIRAGANA,
    /* 94  KEY_MUHENKAN       */ VC_UNDEFINED,
    /* 95  KEY_KPJPCOMMA      */ VC_COMMA,
    /* 96  KEY_KPENTER        */ VC_KP_ENTER,
    /* 97  KEY_RIGHTCTRL      */ VC_CONTROL_R,
    /* 98  KEY_KPSLASH        */ VC_KP_DIVIDE,
    /* 99  KEY_SYSRQ          */ VC_PRINT_SCREEN,
    /* 100 KEY_RIGHTALT       */ VC_ALT_R,
    /* 101 KEY_LINEFEED       */ VC_UNDEFINED,
    /* 102 KEY_HOME           */ VC_HOME,
    /* 103 KEY_UP             */ VC_UP,
    /* 104 KEY_PAGEUP         */ VC_PAGE_UP,
    /* 105 KEY_LEFT           */ VC_LEFT,
    /* 106 KEY_RIGHT          */ VC_RIGHT,
    /* 107 KEY_END            */ VC_END,
    /* 108 KEY_DOWN           */ VC_DOWN,
    /* 109 KEY_PAGEDOWN       */ VC_PAGE_DOWN,
    /* 110 KEY_INSERT         */ VC_INSERT,
    /* 111 KEY_DELETE         */ VC_DELETE,
    /* 112 KEY_MACRO          */ VC_UNDEFINED,
    /* 113 KEY_MUTE           */ VC_VOLUME_MUTE,
    /* 114 KEY_VOLUMEDOWN     */ VC_VOLUME_DOWN,
    /* 115 KEY_VOLUMEUP       */ VC_VOLUME_UP,
    /* 116 KEY_POWER          */ VC_POWER,
    /* 117 KEY_KPEQUAL        */ VC_KP_EQUALS,
    /* 118 KEY_KPPLUSMINUS    */ VC_UNDEFINED,
    /* 119 KEY_PAUSE          */ VC_PAUSE,
    /* 120 KEY_SCALE          */ VC_UNDEFINED,
    /* 121 KEY_KPCOMMA        */ VC_KP_DECIMAL,
    /* 122 KEY_HANGEUL        */ VC_UNDEFINED,
    /* 123 KEY_HANJA          */ VC_UNDEFINED,
    /* 124 KEY_YEN            */ VC_YEN,
    /* 125 KEY_LEFTMETA       */ VC_META_L,
    /* 126 KEY_RIGHTMETA      */ VC_META_R,
    /* 127 KEY_COMPOSE        */ VC_CONTEXT_MENU,
    /* 128 KEY_STOP           */ VC_STOP,
    /* 129 KEY_AGAIN          */ VC_AGAIN,
    /* 130 KEY_PROPS          */ VC_PROPS,
    /* 131 KEY_UNDO           */ VC_UNDO,
    /* 132 KEY_FRONT          */ VC_FRONT,
    /* 133 KEY_COPY           */ VC_COPY,
    /* 134 KEY_OPEN           */ VC_OPEN,
    /* 135 KEY_PASTE          */ VC_INSERT,
    /* 136 KEY_FIND           */ VC_FIND,
    /* 137 KEY_CUT            */ VC_CUT,
    /* 138 KEY_HELP           */ VC_HELP,
    /* 139 KEY_MENU           */ VC_CONTEXT_MENU,
    /* 140 KEY_CALC           */ VC_APP_CALCULATOR,
    /* 141 KEY_SETUP          */ VC_UNDEFINED,
    /* 142 KEY_SLEEP          */ VC_SLEEP,
    /* 143 KEY_WAKEUP         */ VC_WAKE,
    /* 144 KEY_FILE           */ VC_UNDEFINED,
    /* 145 KEY_SENDFILE       */ VC_UNDEFINED,
    /* 146 KEY_DELETEFILE     */ VC_UNDEFINED,
    /* 147 KEY_XFER           */ VC_UNDEFINED,
    /* 148 KEY_PROG1          */ VC_UNDEFINED,
    /* 149 KEY_PROG2          */ VC_UNDEFINED,
    /* 150 KEY_WWW            */ VC_BROWSER_HOME,
    /* 151 KEY_MSDOS          */ VC_UNDEFINED,
    /* 152 KEY_COFFEE         */ VC_UNDEFINED,
    /* 153 KEY_ROTATE_DISPLAY */ VC_UNDEFINED,
    /* 154 KEY_CYCLEWINDOWS   */ VC_UNDEFINED,
    /* 155 KEY_MAIL           */ VC_APP_MAIL,
    /* 156 KEY_BOOKMARKS      */ VC_BROWSER_FAVORITES,
    /* 157 KEY_COMPUTER       */ VC_UNDEFINED,
    /* 158 KEY_BACK           */ VC_BROWSER_BACK,
    /* 159 KEY_FORWARD        */ VC_BROWSER_FORWARD,
    /* 160 KEY_CLOSECD        */ VC_UNDEFINED,
    /* 161 KEY_EJECTCD        */ VC_MEDIA_EJECT,
    /* 162 KEY_EJECTCLOSECD   */ VC_UNDEFINED,
    /* 163 KEY_NEXTSONG       */ VC_MEDIA_NEXT,
    /* 164 KEY_PLAYPAUSE      */ VC_MEDIA_PLAY,
    /* 165 KEY_PREVIOUSSONG   */ VC_MEDIA_PREVIOUS,
    /* 166 KEY_STOPCD         */ VC_MEDIA_STOP,
    /* 167 KEY_RECORD         */ VC_UNDEFINED,
    /* 168 KEY_REWIND         */ VC_UNDEFINED,
    /* 169 KEY_PHONE          */ VC_UNDEFINED,
    /* 170 KEY_ISO            */ VC_UNDEFINED,
    /* 171 KEY_CONFIG         */ VC_UNDEFINED,
    /* 172 KEY_HOMEPAGE       */ VC_BROWSER_HOME,
    /* 173 KEY_REFRESH        */ VC_BROWSER_REFRESH,
    /* 174 KEY_EXIT           */ VC_UNDEFINED,
    /* 175 KEY_MOVE           */ VC_UNDEFINED,
    /* 176 KEY_EDIT           */ VC_UNDEFINED,
    /* 177 KEY_SCROLLUP       */ VC_UNDEFINED,
    /* 178 KEY_SCROLLDOWN     */ VC_UNDEFINED,
    /* 179 KEY_KPLEFTPAREN    */ VC_UNDEFINED,
    /* 180 KEY_KPRIGHTPAREN   */ VC_UNDEFINED,
    /* 181 KEY_NEW            */ VC_UNDEFINED,
    /* 182 KEY_REDO           */ VC_UNDEFINED,
    /* 183 KEY_F13            */ VC_F13,
    /* 184 KEY_F14            */ VC_F14,
    /* 185 KEY_F15            */ VC_F15,
    /* 186 KEY_F16            */ VC_F16,
    /* 187 KEY_F17            */ VC_F17,
    /* 188 KEY_F18            */ VC_F18,
    /* 189 KEY_F19            */ VC_F19,
    /* 190 KEY_F20            */ VC_F20,
    /* 191 KEY_F21            */ VC_F21,
    /* 192 KEY_F22            */ VC_F22,
    /* 193 KEY_F23            */ VC_F23,
    /* 194 KEY_F24            */ VC_F24,
};
// clang-format on

static constexpr size_t TABLE_SIZE = sizeof(evdev_to_uiohook_table) / sizeof(evdev_to_uiohook_table[0]);

uint16_t evdev_to_uiohook(uint16_t evdev_code)
{
    if (evdev_code < TABLE_SIZE) {
        return evdev_to_uiohook_table[evdev_code];
    }

    // Handle codes beyond the table with a switch for common multimedia /
    // special keys that have large evdev codes.
    switch (evdev_code) {
    case KEY_SEARCH:
        return VC_BROWSER_SEARCH;
    case KEY_MEDIA:
        return VC_MEDIA_SELECT;
    default:
        return VC_UNDEFINED;
    }
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