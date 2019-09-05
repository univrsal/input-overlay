/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

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

#define CFG_POS "_pos"
#define CFG_MAPPING "_mapping"
#define CFG_TYPE "_type"
#define CFG_KEY_CODE "_code"
#define CFG_WIDTH "_width"
#define CFG_HEIGHT "_height"
#define CFG_Z_LEVEL "_z_level"
#define CFG_FIRST_ID "first_element"
#define CFG_NEXT_ID "_next"
#define CFG_SIDE "_side"
#define CFG_STICK_RADIUS "_stick_radius"
#define CFG_MOUSE_RADIUS "_mouse_radius"
#define CFG_MOUSE_TYPE "_mouse_type"
#define CFG_DIRECTION "_direction"
#define CFG_TRIGGER_MODE "_trigger_mode"

/* Misc */
#define AXIS_MAX_AMPLITUDE 32767
#define STICK_DEAD_ZONE 100
/* ns after last scroll message until reset */
#define SCROLL_TIMEOUT (120 * 1000 * 1000)

#define TRIGGER_THRESHOLD 0.2f

enum overlay_flag {
	OF_LEFT_STICK = 1 << 0,
	OF_RIGHT_STICK = 1 << 1,
	OF_GAMEPAD = 1 << 2,
	OF_MOUSE = 1 << 3
};

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

enum element_side { ES_INVALID = -1, ES_LEFT, ES_RIGHT };

enum direction { DIR_NONE = 0, DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT, DIR_MAX };

enum button_state { BS_RELEASED = 0, BS_PRESSED = 1 };

enum element_type {
	ET_INVALID = -1,
	ET_TEXTURE,
	ET_BUTTON,
	ET_WHEEL,
	ET_MOUSE_STATS,
	ET_ANALOG_STICK,
	ET_TRIGGER,
	ET_GAMEPAD_ID,
	/* Shows game pad number 1 through 4 */
	ET_DPAD_STICK,
};
