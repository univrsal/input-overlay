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

#ifndef IO_KEY_CODES
#define IO_KEY_CODES

#include <uiohook.h>
/* clang-format off */
#define WHEEL_UP           -1
#define WHEEL_DOWN      	1

/* These were free in uiohook.h */
#define VC_NONE             0xFFFFu

#define VC_MOUSE_MASK       0xED00u /* 'u' for unsigned to fix clang tidy warning */
#define VC_MOUSE_WHEEL_UP   0xED11u
#define VC_MOUSE_WHEEL_DOWN 0xED12u
#define VC_MOUSE_WHEEL      (MOUSE_BUTTON3 | VC_MOUSE_MASK)

/* Contains mouse position */
#define VC_MOUSE_DATA       0xED14u

#define VC_MOUSE_BUTTON1    (MOUSE_BUTTON1 | VC_MOUSE_MASK)
#define VC_MOUSE_BUTTON2    (MOUSE_BUTTON2 | VC_MOUSE_MASK)
#define VC_MOUSE_BUTTON3    VC_MOUSE_WHEEL
#define VC_MOUSE_BUTTON4    (MOUSE_BUTTON4 | VC_MOUSE_MASK)
#define VC_MOUSE_BUTTON5    (MOUSE_BUTTON5 | VC_MOUSE_MASK)

/* These are used for Command mode */
#define CHAR_BACK           0x8
#define CHAR_ENTER          0xD

/* Gamepad constants */
#define VC_PAD_MASK         0xEC00u /* 'u' for unsigned to fix clang tidy warning */
#define VC_STICK_DATA       0xEC30u
#define VC_TRIGGER_DATA     0xEC31u
#define VC_DPAD_DATA        0xEC32u

#define PAD_TO_VC(a)        (a | VC_PAD_MASK)
#define PAD_COUNT 			4

#define VC_PAD_A            ( 0u | VC_PAD_MASK)
#define VC_PAD_B            ( 1u | VC_PAD_MASK)
#define VC_PAD_X            ( 2u | VC_PAD_MASK)
#define VC_PAD_Y            ( 3u | VC_PAD_MASK)
#define VC_PAD_LB           ( 4u | VC_PAD_MASK)
#define VC_PAD_RB           ( 5u | VC_PAD_MASK)
#define VC_PAD_BACK         ( 6u | VC_PAD_MASK)
#define VC_PAD_START        ( 7u | VC_PAD_MASK)
#define VC_PAD_GUIDE        ( 8u | VC_PAD_MASK)
#define VC_PAD_L_ANALOG     ( 9u | VC_PAD_MASK)
#define VC_PAD_R_ANALOG     (10u | VC_PAD_MASK)
#define VC_PAD_DPAD_LEFT    (11u | VC_PAD_MASK)
#define VC_PAD_DPAD_RIGHT   (12u | VC_PAD_MASK)
#define VC_PAD_DPAD_UP      (13u | VC_PAD_MASK)
#define VC_PAD_DPAD_DOWN	(14u | VC_PAD_MASK)
#define VC_PAD_LT           (15u | VC_PAD_MASK)
#define VC_PAD_RT           (16u | VC_PAD_MASK)

#define STICK_MAX_VAL       32767.f
#if LINUX
#define TRIGGER_MAX_VAL		STICK_MAX_VAL
#else
#define TRIGGER_MAX_VAL		256.f
#endif
/* clang-format on */
#endif
