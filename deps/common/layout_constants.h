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

/* Prevent cropped textures from overlapping */
#define CFG_INNER_BORDER 3
#define CFG_OUTER_BORDER 1

#define CFG_DEBUG_FLAG "debug"
#define CFG_DEFAULT_WIDTH "default_width"
#define CFG_DEFAULT_HEIGHT "default_height"
#define CFG_H_SPACE "space_h"
#define CFG_V_SPACE "space_v"

#define CFG_FLAGS "flags"

#define CFG_TOTAL_WIDTH "overlay_width"
#define CFG_TOTAL_HEIGHT "overlay_height"

#define CFG_ID "id"
#define CFG_POS "pos"
#define CFG_MAPPING "mapping"
#define CFG_TYPE "type"
#define CFG_KEY_CODE "code"
#define CFG_WIDTH "width"
#define CFG_HEIGHT "height"
#define CFG_Z_LEVEL "z_level"
#define CFG_ELEMENTS "elements"
#define CFG_SIDE "side"
#define CFG_STICK_RADIUS "stick_radius"
#define CFG_MOUSE_RADIUS "mouse_radius"
#define CFG_MOUSE_TYPE "mouse_type"
#define CFG_DIRECTION "direction"
#define CFG_TRIGGER_MODE "trigger_mode"

/* Misc */
#define PAD_COUNT 4
#define AXIS_MAX_AMPLITUDE 32767
#define STICK_DEAD_ZONE 100

#define TRIGGER_THRESHOLD 0.2f

#undef DT_CENTER /* God dammit microsoft what the fuck */
#undef DT_LEFT
#undef DT_RIGHT
#undef ES_LEFT
#undef ES_RIGHT

enum overlay_flag { OF_LEFT_STICK = 1 << 0, OF_RIGHT_STICK = 1 << 1, OF_GAMEPAD = 1 << 2, OF_MOUSE = 1 << 3 };

enum dpad_texture {
    DT_CENTER,
    DT_LEFT,
    DT_RIGHT,
    DT_UP,
    DT_DOWN,
    DT_TOP_LEFT,
    DT_TOP_RIGHT,
    DT_BOTTOM_LEFT,
    DT_BOTTOM_RIGHT
};

enum dpad_direction {
    DD_CENTER = 1 << 0,
    DD_LEFT = 1 << 1,
    DD_RIGHT = 1 << 2,
    DD_UP = 1 << 3,
    DD_DOWN = 1 << 4,
};

enum mouse_movement { MM_DOT, MM_ARROW };

enum class element_side { INVALID = -1, LEFT, RIGHT };

enum direction { DIR_NONE = 0, DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT, DIR_MAX };

enum button_state { BS_RELEASED = 0, BS_PRESSED = 1 };

enum element_type {
    ET_INVALID = -1,
    ET_TEXTURE,
    ET_KEYBOARD_KEY,
    ET_GAMEPAD_BUTTON,
    ET_MOUSE_BUTTON,
    ET_WHEEL,
    ET_ANALOG_STICK,
    ET_TRIGGER,
    /* Shows game pad number 1 through 4 */
    ET_GAMEPAD_ID,
    ET_DPAD_STICK,
    ET_MOUSE_MOVEMENT
};
