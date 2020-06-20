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

#pragma once
#include "keycodes.h"
#include <gamepad/device.hpp>
#include <stdint.h>
#include <string>
#include <uiohook.h>

/* clang-format off */
#define UTIL_MAX(a, b)                  (((a) > (b)) ? (a) : (b))
#define UTIL_MIN(a, b)                  (((a) < (b)) ? (a) : (b))
#define UTIL_CLAMP(lower, x, upper)     (UTIL_MIN(upper, UTIL_MAX(x, lower)))
#define UTIL_SWAP_BE16(i)               ((i >> 8) | (i << 8))

#define DEAD_ZONE(x, dz)                ((x < dz) && (x > -dz))
/* clang-format on */

namespace common {

inline void util_enable_mask(uint16_t &masks, const uint16_t mask)
{
	masks |= mask;
}

inline void util_disable_mask(uint16_t &masks, const uint16_t mask)
{
	masks &= ~mask;
}

inline void util_set_mask(uint16_t &masks, const uint16_t mask, const bool state)
{
	if (state) {
		util_enable_mask(masks, mask);
	} else {
		util_disable_mask(masks, mask);
	}
}

inline uint16_t util_mouse_to_vc(int m)
{
#ifndef _WIN32 /* Linux mixes right mouse and middle mouse or is windows getting it wrong? */
	if (m == 3)
		m = 2;
	else if (m == 2)
		m = 3;
#endif
	return static_cast<uint16_t>(VC_MOUSE_MASK | m);
}

inline bool ends_with(std::string const &value, std::string const &ending)
{
	if (ending.size() > value.size())
		return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

/* Get default key names from a libuiohook keycode */
static const char *key_to_text(const int key_code)
{
	switch (key_code) {
	case VC_KP_0:
		return "Numpad 0";
	case VC_KP_1:
		return "Numpad 1";
	case VC_KP_2:
		return "Numpad 2";
	case VC_KP_3:
		return "Numpad 3";
	case VC_KP_4:
		return "Numpad 4";
	case VC_KP_5:
		return "Numpad 5";
	case VC_KP_6:
		return "Numpad 6";
	case VC_KP_7:
		return "Numpad 7";
	case VC_KP_8:
		return "Numpad 8";
	case VC_KP_9:
		return "Numpad 9";
	case VC_NUM_LOCK:
		return "Num Lock";
	case VC_KP_MULTIPLY:
		return "Multiply";
	case VC_KP_ADD:
		return "Add";
	case VC_KP_SUBTRACT:
		return "Subtract";
	case VC_KP_COMMA:
		return "Decimal";
	case VC_KP_DIVIDE:
		return "Divide";
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
		return "L-Shift";
	case VC_SHIFT_R:
		return "R-Shift";
	case VC_CONTROL_L:
		return "L-Control";
	case VC_CONTROL_R:
		return "R-Control";
	case VC_ALT_L:
		return "L-Alt";
	case VC_ALT_R:
		return "R-Alt";
	case VC_META_L:
		return "L-Win";
	case VC_META_R:
		return "R-Win";
	case VC_ENTER:
	case VC_KP_ENTER:
		return "Enter";
	case VC_SPACE:
		return "Space";
	case VC_TAB:
		return "Tab";
	case VC_BACKSPACE:
		return "Backspace";
	case VC_ESCAPE:
		return "Esc";
	case VC_INSERT:
		return "Insert";
	case VC_HOME:
		return "Home";
	case VC_PAGE_UP:
		return "Page up";
	case VC_PAGE_DOWN:
		return "Page down";
	case VC_END:
		return "End";
	case VC_DELETE:
		return "Delete";
	case VC_UP:
	case VC_KP_UP:
		return "Up";
	case VC_DOWN:
	case VC_KP_DOWN:
		return "Down";
	case VC_LEFT:
	case VC_KP_LEFT:
		return "Left";
	case VC_RIGHT:
	case VC_KP_RIGHT:
		return "Right";
	case VC_PRINTSCREEN:
		return "Print";
	case VC_SCROLL_LOCK:
		return "Scroll lock";
	case VC_PAUSE:
		return "Pause";
	case VC_CAPS_LOCK:
		return "Capslock";
	case VC_MOUSE_BUTTON1:
		return "Left mouse";
	case VC_MOUSE_BUTTON2:
		return "Right mouse";
	case VC_MOUSE_BUTTON3:
		return "Middle mouse";
	case VC_MOUSE_BUTTON4:
		/* If you have a better name for them lemme know */
		return "Mouse4";
	case VC_MOUSE_BUTTON5:
		return "Mouse5";
	case VC_MOUSE_WHEEL_UP:
		return "Scroll up";
	case VC_MOUSE_WHEEL_DOWN:
		return "Scroll down";
	case gamepad::button::A:
		return "Pad A";
	case gamepad::button::B:
		return "Pad B";
	case gamepad::button::X:
		return "Pad X";
	case gamepad::button::Y:
		return "Pad Y";
	case gamepad::button::LB:
		return "Pad LB";
	case gamepad::button::RB:
		return "Pad RB";
	case gamepad::button::BACK:
		return "Back";
	case gamepad::button::START:
		return "Start";
	case gamepad::button::GUIDE:
		return "X-Box Button";
	case gamepad::button::L_THUMB:
		return "Left Stick";
	case gamepad::button::R_THUMB:
		return "Right Stick";
	case gamepad::button::DPAD_LEFT:
		return "DPad Left";
	case gamepad::button::DPAD_RIGHT:
		return "DPad Right";
	case gamepad::button::DPAD_UP:
		return "DPad Up";
	case gamepad::button::DPAD_DOWN:
		return "DPad Down";
	case gamepad::axis::LEFT_TRIGGER:
		return "LT";
	case gamepad::axis::RIGHT_TRIGGER:
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

}
