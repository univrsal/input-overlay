/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.xyz/alex/input-overlay
 * Copyright 2023 Alex <uni@vrsal.xyz>.
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
#include <uiohook.h>
/* clang-format off */
#define WHEEL_UP           -1
#define WHEEL_DOWN          1

/* These were free in uiohook.h */
#define VC_NONE             0xFFFFu

#define VC_MOUSE_MASK       0xED00u /* 'u' for unsigned to fix clang tidy warning */
#define VC_MOUSE_WHEEL      (MOUSE_BUTTON3 | VC_MOUSE_MASK)

/* Contains mouse position */
#define VC_MOUSE_DATA       0xED14u

#define VC_MOUSE_BUTTON1    (MOUSE_BUTTON1 | VC_MOUSE_MASK)
#define VC_MOUSE_BUTTON2    (MOUSE_BUTTON2 | VC_MOUSE_MASK)
#define VC_MOUSE_BUTTON3    VC_MOUSE_WHEEL
#define VC_MOUSE_BUTTON4    (MOUSE_BUTTON4 | VC_MOUSE_MASK)
#define VC_MOUSE_BUTTON5    (MOUSE_BUTTON5 | VC_MOUSE_MASK)

/* Gamepad constants */
#define VC_PAD_MASK         0xEC00u /* 'u' for unsigned to fix clang tidy warning */
#define VC_STICK_DATA       0xEC30u
#define VC_TRIGGER_DATA     0xEC31u
#define VC_DPAD_DATA        0xEC32u

/* clang-format on */
