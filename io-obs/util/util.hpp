/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#ifndef CCT

#ifdef LINUX
#define STICK_MAX_VAL   127.f
#include <math.h>
#elif _WIN32
#define STICK_MAX_VAL       32767.f
#endif

#define TRIGGER_MAX_VAL     256.f

#include <string>
#include <vector>

#ifndef IO_CLIENT /* io-client uses this header, but doesn't use any obs headers */
#include <obs-module.h>
#define T_(v)                           obs_module_text(v)
#define GET_PROPS(S)                    (obs_properties_get(props, S))
#define GET_BOOL(X)                     (obs_data_get_bool(s, X))
#else
#define T_(v)
#endif

#ifdef DEBUG
#include <random>
#endif
#endif /* CCT */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define UTIL_MAX(a, b)                  (((a) > (b)) ? (a) : (b))
#define UTIL_MIN(a, b)                  (((a) < (b)) ? (a) : (b))
#define UTIL_CLAMP(lower, x, upper)     (UTIL_MIN(upper, UTIL_MAX(x, lower)))
#define UTIL_SWAP_BE16(i)               ((i >> 8) | (i << 8))

#define DEAD_ZONE(x, dz)                ((x < dz) && (x > -dz))

/* Settings values*/
#define S_REGION                        "input-overlay"
#define S_UIOHOOK                       "iohook"
#define S_GAMEPAD                       "gamepad"
#define S_OVERLAY                       "overlay"
#define S_HISTORY                       "history"
#define S_REMOTE                        "remote"
#define S_LOGGING                       "logging"
#define S_PORT                          "port"
#define S_REFRESH                       "refresh_rate"
#define S_CONTROL                       "control"
#define S_REGEX                         "regex"
#define S_FILTER_MODE                   "filter_mode"
#define S_FILTER_COUNT                  "filter_count"
#define S_FILTER_BASE                   "filter_"

/* Linux gamepad bindings TODO: Maybe for windows as well? */
#define S_BINDING_A                     "binding_a"
#define S_BINDING_B                     "binding_b"
#define S_BINDING_X                     "binding_x"
#define S_BINDING_Y                     "binding_y"
#define S_BINDING_START                 "binding_start"
#define S_BINDING_BACK                  "binding_back"
#define S_BINDING_GUIDE                 "binding_guide"
#define S_BINDING_RB                    "binding_rb"
#define S_BINDING_LB                    "binding_lb"
#define S_BINDING_RT                    "binding_rt"
#define S_BINDING_LT                    "binding_lt"
#define S_BINDING_DPAD_UP               "binding_dpad_up"
#define S_BINDING_DPAD_DOWN             "binding_dpad_down"
#define S_BINDING_DPAD_LEFT             "binding_dpad_left"
#define S_BINDING_DPAD_RIGHT            "binding_dpad_right"
#define S_BINDING_ANALOG_L              "binding_analog_l"
#define S_BINDING_ANALOG_R              "binding_analog_r"
#define S_BINDING_ANALOG_LX             "binding_analog_lx"
#define S_BINDING_ANALOG_LY             "binding_analog_ly"
#define S_BINDING_ANALOG_RX             "binding_analog_rx"
#define S_BINDING_ANALOG_RY             "binding_analog_ry"

/* === Source properties === */

/* Common values */
#define S_INPUT_SOURCE                  "io.input_source"
#define S_RELOAD_CONNECTIONS            "io.reload_connections"

#define T_LOCAL_SOURCE                  T_("Source.InputSource.Local")
#define T_INPUT_SOURCE                  T_("Source.InputSource")
#define T_RELOAD_CONNECTIONS            T_("Source.InputSource.Reload")

/* Lang Input Overlay */
#define S_OVERLAY_FILE                  "io.overlay_image"
#define S_LAYOUT_FILE                   "io.layout_file"
#define S_CONTROLLER_ID                 "io.controller_id"
#define S_CONTROLLER_L_DEAD_ZONE        "io.controller_l_deadzone"
#define S_CONTROLLER_R_DEAD_ZONE        "io.controller_r_deadzone"
#define S_MOUSE_SENS                    "io.mouse_sens"
#define S_MOUSE_DEAD_ZONE               "io.mouse_deadzone"
#define S_MONITOR_USE_CENTER            "io.monitor_use_center"
#define S_MONITOR_H_CENTER              "io.monitor_h_center"
#define S_MONITOR_V_CENTER              "io.monitor_v_center"
#define S_RELOAD_PAD_DEVICES            "io.reload_pads"

#define T_TEXTURE_FILE                  T_("Overlay.Path.Texture")
#define T_LAYOUT_FILE                   T_("Overlay.Path.Layout")
#define T_FILTER_IMAGE_FILES            T_("Filter.ImageFiles")
#define T_FILTER_TEXT_FILES             T_("Filter.TextFiles")
#define T_FILTER_ALL_FILES              T_("Filter.AllFiles")
#define T_RELOAD_PAD_DEVICES            T_("Gamepad.Reload")
#define T_CONTROLLER_ID                 T_("Gamepad.Id")
#define T_CONROLLER_L_DEADZONE          T_("Gamepad.LeftDeadZone")
#define T_CONROLLER_R_DEADZONE          T_("Gamepad.RightDeadZone")
#define T_MOUSE_SENS                    T_("Mouse.Sensitivity")
#define T_MOUSE_DEAD_ZONE               T_("Mouse.Deadzone")
#define T_MONITOR_USE_CENTER            T_("Mouse.UseCenter")
#define T_MONITOR_H_CENTER              T_("Monitor.CenterX")
#define T_MONITOR_V_CENTER              T_("Monitor.CenterY")

/* Lang Input History */
#define S_HISTORY_SIZE                  "io.history_size"
#define S_HISTORY_FIX_CUTTING           "io.fix_cutting"
#define S_HISTORY_INCLUDE_MOUSE         "io.include_mouse"
#define S_HISTORY_INCLUDE_PAD           "io.include_pad"
#define S_HISTORY_INTERVAL              "io.interval"
#define S_HISTORY_CLEAR_HISTORY         "io.clear_history"
#define S_HISTORY_ENABLE_REPEAT_KEYS    "io.repeat_keys"
#define S_HISTORY_ENABLE_AUTO_CLEAR     "io.auto_clear"
#define S_HISTORY_AUTO_CLEAR_INTERVAL   "io.auto_clear_interval"
#define S_HISTORY_ICON_V_SPACE          "io.icon_v_space"
#define S_HISTORY_ICON_H_SPACE          "io.icon_h_space"

#define S_HISTORY_MODE                  "io.mode"
#define S_HISTORY_KEY_NAME_PATH         "io.key_name_path"
#define S_HISTORY_USE_FALLBACK_NAME     "io.use_fallback_names"
#define S_HISTORY_DIRECTION             "io.direction"
#define S_HISTORY_DIRECTION_UP          "io.up"
#define S_HISTORY_DIRECTION_DOWN        "io.down"
#define S_HISTORY_DIRECTION_LEFT        "io.left"
#define S_HISTORY_DIRECTION_RIGHT       "io.right"

#define S_HISTORY_KEY_ICON_PATH         "io.key_icon_path"
#define S_HISTORY_KEY_ICON_CONFIG_PATH  "io.key_icon_config"

#define S_HISTORY_SHOW_FONT             "io.show_font"

#define T_HISTORY_USE_FALLBACK_NAMES    T_("History.UseFallbackNames")
#define T_HISTORY_KEY_NAME_PATH         T_("History.Path.KeyNames")
#define T_HISTORY_KEY_ICON_PATH         T_("History.Path.Icons.Texture")
#define T_HISTORY_KEY_ICON_CONFIG_PATH  T_("History.Path.Icons.Config")
#define T_HISTORY_ICON_V_SPACE          T_("History.Icons.Space.Vertical")
#define T_HISTORY_ICON_H_SPACE          T_("History.Icons.Space.Horizontal")

#define T_HISTORY_MODE                  T_("History.Mode")
#define T_HISTORY_MODE_TEXT             T_("History.Mode.Text")
#define T_HISTORY_MODE_ICON             T_("History.Mode.Icons")
#define T_HISTORY_COMMAND_MODE          T_("History.Mode.Commands") /* TODO: reimplement? */
#define T_HISTORY_DIRECTION             T_("History.Direction")
#define T_HISTORY_DIRECTION_UP          T_("History.Direction.Up")
#define T_HISTORY_DIRECTION_DOWN        T_("History.Direction.Down")
#define T_HISTORY_DIRECTION_LEFT        T_("History.Direction.Left")
#define T_HISTORY_DIRECTION_RIGHT       T_("History.Direction.Right")

#define T_HISTORY_SHOW_FONT             T_("History.Font.Show")

#define T_HISTORY_DIRECTION             T_("History.Direction")
#define T_HISTORY_HISTORY_SIZE          T_("History.Size")
#define T_HISTORY_FIX_CUTTING           T_("History.FixCutting")
#define T_HISTORY_INCLUDE_MOUSE         T_("History.IncludeMouse")
#define T_HISTORY_INCLUDE_PAD           T_("History.IncludePad")
#define T_HISTORY_CLEAR_HISTORY         T_("History.ClearHistory")

#define T_HISTORY_INTERVAL              T_("History.Update.Interval")
#define T_HISTORY_ENABLE_REPEAT_KEYS    T_("History.Enable.RepeatKeys")
#define T_HISTORY_ENABLE_AUTO_CLEAR     T_("History.Enable.AutoClear")
#define T_HISTORY_AUTO_CLEAR_INTERVAL   T_("History.AutoClear.Interval")

#define T_MENU_OPEN_SETTINGS            T_("Menu.InputOverlay.OpenSettings")
#define T_REFRESH_RATE_TOOLTIP          T_("Dialog.InputOverlay.RemoteRefreshRate.Tooltip")

#define WHEEL_UP       -1
#define WHEEL_DOWN      1

/* These were free in uiohook.h */
#define VC_NONE                         0xFFFFu

#define VC_MOUSE_MASK                   0xED00u /* 'u' for unsigned to fix clang tidy warning */
#define VC_MOUSE_WHEEL_UP               0xED11u
#define VC_MOUSE_WHEEL_DOWN             0xED12u
#define VC_MOUSE_WHEEL                 	(MOUSE_BUTTON3 | VC_MOUSE_MASK)
/* Contains mouse position */
#define VC_MOUSE_DATA                   0xED14u

#define VC_MOUSE_BUTTON1                (MOUSE_BUTTON1 | VC_MOUSE_MASK)
#define VC_MOUSE_BUTTON2                (MOUSE_BUTTON2 | VC_MOUSE_MASK)
#define VC_MOUSE_BUTTON3                VC_MOUSE_WHEEL
#define VC_MOUSE_BUTTON4                (MOUSE_BUTTON4 | VC_MOUSE_MASK)
#define VC_MOUSE_BUTTON5                (MOUSE_BUTTON5 | VC_MOUSE_MASK)

/* These are used for Command mode */
#define CHAR_BACK                       0x8
#define CHAR_ENTER                      0xD

/* Gamepad constants */
#define VC_PAD_MASK                     0xEC00u /* 'u' for unsigned to fix clang tidy warning */
#define VC_STICK_DATA                   0xEC30u
#define VC_TRIGGER_DATA                 0xEC31u
#define VC_DPAD_DATA                    0xEC32u

#define PAD_TO_VC(a)                    (a | VC_PAD_MASK)
#define PAD_COUNT 4

#define VC_PAD_A                        ( 0u | VC_PAD_MASK)
#define VC_PAD_B                        ( 1u | VC_PAD_MASK)
#define VC_PAD_X                        ( 2u | VC_PAD_MASK)
#define VC_PAD_Y                        ( 3u | VC_PAD_MASK)
#define VC_PAD_LB                       ( 4u | VC_PAD_MASK)
#define VC_PAD_RB                       ( 5u | VC_PAD_MASK)
#define VC_PAD_BACK                     ( 6u | VC_PAD_MASK)
#define VC_PAD_START                    ( 7u | VC_PAD_MASK)
#define VC_PAD_GUIDE                    ( 8u | VC_PAD_MASK)
#define VC_PAD_L_ANALOG                 ( 9u | VC_PAD_MASK)
#define VC_PAD_R_ANALOG                 (10u | VC_PAD_MASK)
#define VC_PAD_DPAD_LEFT                (11u | VC_PAD_MASK)
#define VC_PAD_DPAD_RIGHT               (12u | VC_PAD_MASK)
#define VC_PAD_DPAD_UP                  (13u | VC_PAD_MASK)
#define VC_PAD_DPAD_DOWN                (14u | VC_PAD_MASK)
#define VC_PAD_LT                       (15u | VC_PAD_MASK)
#define VC_PAD_RT                       (16u | VC_PAD_MASK)

/* Get default key names from a libuiohook keycode */
const char* key_to_text(int key_code);

/* Creates string for obs to use as accepted files for a file dialog */
std::string util_file_filter(const char* display, const char* formats);

/* Changes slashes in path to fit Unix formatting */
void util_format_path(std::string &path);

void util_enable_mask(uint16_t &masks, uint16_t mask);

void util_disable_mask(uint16_t &masks, uint16_t mask);

void util_set_mask(uint16_t &masks, uint16_t mask, bool state);

uint16_t util_mouse_to_vc(int m);

inline bool ends_with(std::string const &value, std::string const &ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

/* Source:
 * github.com/obsproject/obs-studio/blob/master/UI/frontend-plugins/frontend-tools/auto-scene-switcher-win.cpp
 * github.com/obsproject/obs-studio/blob/master/UI/frontend-plugins/frontend-tools/auto-scene-switcher-nix.cpp
 * github.com/obsproject/obs-studio/blob/master/UI/frontend-plugins/frontend-tools/auto-scene-switcher-osx.mm
 */
extern void GetWindowList(std::vector<std::string> &windows);

extern void GetCurrentWindowTitle(std::string &title);
