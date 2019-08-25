#ifndef UTIL_H
#define UTIL_H

/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#ifndef NULL
#define NULL 0
#endif // !NULL

#ifndef WINDOWS
#ifdef _WIN32
#define WINDOWS
#include <windows.h>
#else
#ifndef LINUX
#define LINUX
#endif
#endif
#endif

#include <uiohook.h>

#ifdef LINUX

#include <math.h>

#endif

#include <string>
#include <obs-module.h>
#include <algorithm>

#ifdef DEBUG

#include <random>

#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define GET_PROPS(S)    (obs_properties_get(props, S))

#define UTIL_MAX(a, b)               (((a) > (b)) ? (a) : (b))
#define UTIL_MIN(a, b)               (((a) < (b)) ? (a) : (b))
#define UTIL_CLAMP(lower, x, upper) (UTIL_MIN(upper, UTIL_MAX(x, lower)))

#define DEAD_ZONE(x, dz) ((x < dz) && (x > -dz))
#define X_PRESSED(b) ((m_xinput.Gamepad.wButtons & b) != 0)

#define warning(format, ...) blog(LOG_WARNING, "[%s] " format, \
        obs_source_get_name(m_source), ##__VA_ARGS__)

// Lang Input Overlay
#define S_OVERLAY_FILE              "overlay_image"
#define S_LAYOUT_FILE               "layout_file"
#define S_IS_CONTROLLER             "is_controller"
#define S_CONTROLLER_ID             "controller_id"
#define S_CONTROLLER_L_DEAD_ZONE    "controller_l_deadzone"
#define S_CONTROLLER_R_DEAD_ZONE    "controller_r_deadzone"
#define S_CONTROLLER_RELOAD			"controller_reload"
#define S_IS_MOUSE                  "is_mouse"
#define S_MOUSE_SENS                "mouse_sens"
#define S_MOUSE_DEAD_ZONE           "mouse_deadzone"
#define S_MONITOR_USE_CENTER        "monitor_use_center"
#define S_MONITOR_H_CENTER          "monitor_h_center"
#define S_MONITOR_V_CENTER          "monitor_v_center"

#define T_(v)                       obs_module_text(v)
#define T_OVERLAY_FILE              T_("OverlayFile")
#define T_LAYOUT_FILE               T_("LayoutFile")
#define T_FILTER_IMAGE_FILES        T_("Filter.ImageFiles")
#define T_FILTER_TEXT_FILES         T_("Filter.TextFiles")
#define T_FILTER_ALL_FILES          T_("Filter.AllFiles")
#define T_IS_CONTROLLER             T_("Gamepad.IsGamepad")
#define T_CONTROLLER_ID             T_("GamepadId")
#define T_CONTROLLER_RELOAD			T_("Gamepad.Reload")
#define T_CONROLLER_L_DEADZONE      T_("Gamepad.LeftDeadZone")
#define T_CONROLLER_R_DEADZONE      T_("Gamepad.RightDeadZone")
#define T_IS_MOUSE                  T_("Mouse.IsMouse")
#define T_MOUSE_SENS                T_("Mouse.Sensitivity")
#define T_MOUSE_DEAD_ZONE           T_("Mouse.Deadzone")
#define T_MONITOR_USE_CENTER        T_("Mouse.UseCenter")
#define T_MONITOR_H_CENTER          T_("Monitor.CenterX")
#define T_MONITOR_V_CENTER          T_("Monitor.CenterY")

// Lang Input History
#define S_OVERLAY_HISTORY_SIZE          "history_size"
#define S_OVERLAY_FIX_CUTTING           "fix_cutting"
#define S_OVERLAY_INCLUDE_MOUSE         "include_mouse"
#define S_OVERLAY_INTERVAL              "interval"
#define S_OVERLAY_CLEAR_HISTORY         "clear_history"
#define S_OVERLAY_ENABLE_REPEAT_KEYS    "repeat_keys"
#define S_OVERLAY_ENABLE_AUTO_CLEAR     "auto_clear"
#define S_OVERLAY_AUTO_CLEAR_INTERVAL   "auto_clear_interval"
#define S_OVERLAY_ICON_V_SPACE          "icon_v_space"
#define S_OVERLAY_ICON_H_SPACE          "icon_h_space"

#define S_OVERLAY_MODE                  "mode"
#define S_OVERLAY_KEY_NAME_PATH         "key_name_path"
#define S_OVERLAY_USE_FALLBACK_NAME     "use_fallback_names"
#define S_OVERLAY_DIRECTION             "direction"
#define S_OVERLAY_DIRECTION_UP          "up"
#define S_OVERLAY_DIRECTION_DOWN        "down"
#define S_OVERLAY_DIRECTION_LEFT        "left"
#define S_OVERLAY_DIRECTION_RIGHT       "right"

#define S_OVERLAY_KEY_ICON_PATH         "key_icon_path"
#define S_OVERLAY_KEY_ICON_CONFIG_PATH  "key_icon_config"

#define S_OVERLAY_FONT                  "font"
#define S_OVERLAY_FONT_COLOR            "color"
#define S_OVERLAY_OUTLINE               "outline"
#define S_OVERLAY_OUTLINE_SIZE          "outline_size"
#define S_OVERLAY_OUTLINE_COLOR         "outline_color"
#define S_OVERLAY_OUTLINE_OPACITY       "outline_opacity"
#define S_OVERLAY_OPACITY               "opacity"

#define T_OVERLAY_KEY_NAME_PATH         T_("Overlay.KeyTranslationPath")
#define T_OVERLAY_USE_FALLBACK_NAMES    T_("Overlay.UseFallback.Translation")
#define T_OVERLAY_KEY_ICON_PATH         T_("Overlay.KeyIconPath")
#define T_OVERLAY_KEY_ICON_CONFIG_PATH  T_("Overlay.KeyIconConfigPath")
#define T_OVERLAY_ICON_V_SPACE          T_("Overlay.KeyIconVSpace")
#define T_OVERLAY_ICON_H_SPACE          T_("Overlay.KeyIconHSpace")

#define T_OVERLAY_MODE                  T_("Overlay.Mode")
#define T_OVERLAY_MODE_TEXT             T_("Overlay.Mode.Text")
#define T_OVERLAY_MODE_ICON             T_("Overlay.Mode.Icons")
#define T_OVERLAY_DIRECTION             T_("Overlay.Direction")
#define T_OVERLAY_DIRECTION_UP          T_("Overlay.Direction.Up")
#define T_OVERLAY_DIRECTION_DOWN        T_("Overlay.Direction.Down")
#define T_OVERLAY_DIRECTION_LEFT        T_("Overlay.Direction.Left")
#define T_OVERLAY_DIRECTION_RIGHT       T_("Overlay.Direction.Right")

#define T_OVERLAY_FONT                  T_("OverlayFont")
#define T_OVERLAY_FONT_COLOR            T_("OverlayFontColor")
#define T_OVERLAY_DIRECTION_LABEL       T_("OverlayDirection.Label")
#define T_OVERLAY_HISTORY_SIZE          T_("OverlayHistory.Size")
#define T_OVERLAY_FIX_CUTTING           T_("Overlay.FixCutting")
#define T_OVERLAY_INCLUDE_MOUSE         T_("Overlay.IncludeMouse")
#define T_OVERLAY_CLEAR_HISTORY         T_("Overlay.ClearHistory")
#define T_OVERLAY_OPACITY               T_("Overlay.Opacity")

#define T_OVERLAY_OUTLINE               T_("Overlay.Outline")
#define T_OVERLAY_OUTLINE_SIZE          T_("Overlay.Outline.Size")
#define T_OVERLAY_OUTLINE_COLOR         T_("Overlay.Outline.Color")
#define T_OVERLAY_OUTLINE_OPACITY       T_("Overlay.Outline.Opacity")
#define T_OVERLAY_INTERVAL              T_("Overlay.Update.Interval")
#define T_OVERLAY_ENABLE_REPEAT_KEYS    T_("Overlay.Enable.RepeatKeys")
#define T_OVERLAY_ENABLE_AUTO_CLEAR     T_("Overlay.Enable.AutoClear")
#define T_OVERLAY_AUTO_CLEAR_INTERVAL   T_("Overlay.AutoClear.Interval")

#define WHEEL_UP        -1
#define WHEEL_DOWN      1

/* These were free in uiohook.h */
#define VC_NONE               0xffff
#define VC_MOUSE_WHEEL_UP     0xED10
#define VC_MOUSE_WHEEL_DOWN   0xED11
#define VC_MOUSE_MASK         0xED00

#define VC_MOUSE_BUTTON1      (MOUSE_BUTTON1 | VC_MOUSE_MASK)
#define VC_MOUSE_BUTTON2      (MOUSE_BUTTON2 | VC_MOUSE_MASK)
#define VC_MOUSE_BUTTON3      (MOUSE_BUTTON3 | VC_MOUSE_MASK)
#define VC_MOUSE_BUTTON4      (MOUSE_BUTTON4 | VC_MOUSE_MASK)
#define VC_MOUSE_BUTTON5      (MOUSE_BUTTON5 | VC_MOUSE_MASK)

// Gamepad constants
#define PAD_STICK_MAX_VAL   32767
#define PAD_ICON_COUNT      22
#define PAD_BUTTON_COUNT    17

#define PAD_BODY            17
#define PAD_PLAYER_1        18
#define PAD_PLAYER_2        19
#define PAD_PLAYER_3        20
#define PAD_PLAYER_4        21

#define PAD_A               0
#define PAD_B               1
#define PAD_X               2
#define PAD_Y               3
#define PAD_LB              4
#define PAD_RB              5
#define PAD_BACK            6
#define PAD_START           7
#define PAD_X_BOX_KEY       8
#define PAD_L_ANALOG        9
#define PAD_R_ANALOG        10
#define PAD_DPAD_LEFT       11
#define PAD_DPAD_RIGHT      12
#define PAD_DPAD_UP         13
#define PAD_DPAD_DOWN       14
#define PAD_LT              15
#define PAD_RT              16

#define PAD_AXIS_LX			0
#define PAD_AXIS_LY			1
#define PAD_AXIS_LT			2
#define PAD_AXIS_RX			3
#define PAD_AXIS_RY			4
#define PAD_AXIS_RT			5

// Get default keynames from a libuiohook keycode
const char* key_to_text(int key_code);

// Reads first integer off of csv string
uint16_t util_read_int(std::string &l);

// Reads first hex code off of csv string and converts it to int
uint16_t util_read_hex(std::string &l);

// Creates string for obs to use as accepted files for a file dialog
std::string util_file_filter(const char* display, const char* formats);

// Changes slashes in path to fit unix formatting
void util_format_path(std::string &path);

void util_enable_mask(uint8_t &masks, uint8_t mask);

void util_disable_mask(uint8_t &masks, uint8_t mask);

void util_set_mask(uint8_t &masks, uint8_t mask, bool state);

uint16_t util_mouse_to_vc(int m);

#ifdef DEBUG

uint16_t random_vc(void);

#endif

#ifndef WINDOWS
// uiohook doesn't define these on linux
// and I don't want to header guard them
// in the switch case...
#define VC_KP_UP    0xEE48
#define VC_KP_DOWN    0xEE4B
#define VC_KP_LEFT    0xEE4D
#define VC_KP_RIGHT    0xEE50
#endif
#endif
