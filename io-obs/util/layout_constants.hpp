/**
 * Created by univrsal on 06.07.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */
#pragma once

#define CFG_INNER_BORDER    3
#define CFG_OUTTER_BORDER   1

#define CFG_DEFAULT_WIDTH   "default_width"
#define CFG_DEFAULT_HEIGHT  "default_height"
#define CFG_H_SPACE         "space_h"
#define CFG_V_SPACE         "space_v"

#define CFG_TOTAL_WIDTH     "overlay_width"
#define CFG_TOTAL_HEIGHT    "overlay_height"

#define CFG_X_POS           "_x_pos"
#define CFG_Y_POS           "_y_pos"
#define CFG_U               "_u"
#define CFG_V               "_v"
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
#define CFG_TEXT            "_text"
#define CFG_TEXT_RESET      "_reset_text"

/* Misc */
#define AXIS_MAX_AMPLITUDE 32767
#define STICK_DEAD_ZONE     100

/* Text element formatting */
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

enum MouseMovementType
{
    DOT,
    ARROW
};

enum ElementSide
{
    SIDE_LEFT,
    SIDE_RIGHT
};

enum TriggerDirection
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

enum ElementType
{
    INVALID = -1,
    TEXTURE,
    BUTTON,
    /* MOUSE_WHEEL was taken :( */
    MOUSE_SCROLLWHEEL,
    MOUSE_MOVEMENT,
    ANALOG_STICK,
    TRIGGER,
    /* Shows game pad number 1 through 4 */
    GAMEPAD_ID,
    TEXT,
    DPAD_STICK,
};
