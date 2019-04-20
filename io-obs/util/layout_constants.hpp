/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

/* Prevent cropped textures from overlapping */
#define CFG_INNER_BORDER    3
#define CFG_OUTER_BORDER    1

#define CFG_DEBUG_FLAG      "debug"
#define CFG_DEFAULT_WIDTH   "default_width"
#define CFG_DEFAULT_HEIGHT  "default_height"
#define CFG_H_SPACE         "space_h"
#define CFG_V_SPACE         "space_v"

#define CFG_FLAGS           "flags"

#define CFG_TOTAL_WIDTH     "overlay_width"
#define CFG_TOTAL_HEIGHT    "overlay_height"

#define CFG_POS             "_pos"
#define CFG_MAPPING         "_mapping"
#define CFG_TYPE            "_type"
#define CFG_KEY_CODE        "_code"
#define CFG_WIDTH           "_width"
#define CFG_HEIGHT          "_height"
#define CFG_Z_LEVEL         "_z_level"
#define CFG_FIRST_ID        "first_element"
#define CFG_NEXT_ID         "_next"
#define CFG_SIDE            "_side"
#define CFG_STICK_RADIUS    "_stick_radius"
#define CFG_MOUSE_RADIUS    "_mouse_radius"
#define CFG_MOUSE_TYPE      "_mouse_type"
#define CFG_DIRECTION       "_direction"
#define CFG_TRIGGER_MODE    "_trigger_mode"

/* Misc */
#define AXIS_MAX_AMPLITUDE  32767
#define STICK_DEAD_ZONE     100
/* ns after last scroll message until reset */
#define SCROLL_TIMEOUT      (120 * 1000 * 1000)

#define TRIGGER_THRESHOLD   0.2f

/* Text element formatting TODO: remove*/
#define TEXT_FORMAT_WHEEL_AMOUNT    "%w"
#define TEXT_FORMAT_LMB_CLICKS      "%l"
#define TEXT_FORMAT_RMB_CLICKS      "%r"
#define TEXT_FORMAT_MMB_CLICKS      "%m"
#define TEXT_FORMAT_MOUSE_X         "%x"
#define TEXT_FORMAT_MOUSE_Y         "%y"

#define TEXT_FORMAT_WHEEL_FLAG      1 << 0
#define TEXT_FORMAT_LMB_FLAG        1 << 1
#define TEXT_FORMAT_RMB_FLAG        1 << 2
#define TEXT_FORMAT_MMB_FLAG        1 << 3
#define TEXT_FORMAT_MOUSE_X_FLAG    1 << 4
#define TEXT_FORMAT_MOUSE_Y_FLAG    1 << 5

enum overlay_flags
{
    FLAG_LEFT_STICK = 1 << 0, FLAG_RIGHT_STICK = 1 << 1, FLAG_GAMEPAD = 1 << 2, FLAG_MOUSE = 1 << 3
};

enum dpad_texture
{
    DPAD_TEXTURE_CENTER,
    DPAD_TEXTURE_LEFT,
    DPAD_TEXTURE_RIGHT,
    DPAD_TEXTURE_UP,
    DPAD_TEXTURE_DOWN,
    DPAD_TEXTURE_TOP_LEFT,
    DPAD_TEXTURE_TOP_RIGHT,
    DPAD_TEXTURE_BOTTOM_LEFT,
    DPAD_TEXTURE_BOTTOM_RIGHT
};

enum dpad_direction
{
    DPAD_CENTER = 1 << 0,
    DPAD_LEFT = 1 << 1,
    DPAD_RIGHT = 1 << 2,
    DPAD_UP = 1 << 3,
    DPAD_DOWN = 1 << 4,
};

enum mouse_movement_type
{
    DOT, ARROW
};

enum element_side
{
    SIDE_INVALID = -1, SIDE_LEFT, SIDE_RIGHT
};

enum trigger_direction
{
    TRIGGER_UP, TRIGGER_DOWN, TRIGGER_LEFT, TRIGGER_RIGHT
};

enum history_direction
{
    DIR_DOWN, DIR_UP, DIR_LEFT, DIR_RIGHT
};

enum button_state
{
    STATE_RELEASED, STATE_PRESSED,
};

enum wheel_direction
{
    WHEEL_DIR_UP = -1, WHEEL_DIR_NONE = 0, WHEEL_DIR_DOWN = 1
};

enum element_type
{
    INVALID = -1, TEXTURE, BUTTON, /* MOUSE_WHEEL was taken :( */
    MOUSE_SCROLLWHEEL, MOUSE_STATS, ANALOG_STICK, TRIGGER, /* Shows game pad number 1 through 4 */
    GAMEPAD_ID, DPAD_STICK,
};
