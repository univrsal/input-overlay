#ifndef UTIL_H
#define UTIL_H

#ifndef NULL
#define NULL 0
#endif // !NULL

#ifndef WINDOWS
#ifdef _WIN32
#define WINDOWS
#endif
#endif

#ifdef WINDOWS
#include <Windows.h>
#endif

#include <uiohook.h>

#define DEAD_ZONE(x, dz) ((x < dz) && (x > -dz))
#define X_PRESSED(b) ((m_xinput.Gamepad.wButtons & b) != 0)

#define warning(format, ...) blog(LOG_WARNING, "[%s] " format, \
		obs_source_get_name(m_source), ##__VA_ARGS__)

#define SHIFTED 0x8000 

#define KEY_LMB     VK_LBUTTON
#define KEY_RMB     VK_RBUTTON
#define KEY_MMB     VK_MBUTTON
#define KEY_SMB1    VK_XBUTTON1
#define KEY_SMB2    VK_XBUTTON2
#define KEY_NONE    0x07

// Lang Input Overlay
#define S_OVERLAY_FILE              "overlay_image"
#define S_LAYOUT_FILE               "layout_file"
#define S_IS_CONTROLLER             "is_controller"
#define S_CONTROLLER_ID             "controller_id"
#define S_CONTROLLER_L_DEAD_ZONE    "controller_l_deadzone"
#define S_CONTROLLER_R_DEAD_ZONE    "controller_r_deadzone"

#define T_(v)                       obs_module_text(v)
#define T_OVERLAY_FILE              T_("OverlayFile")
#define T_LAYOUT_FILE               T_("LayoutFile")
#define T_FILTER_IMAGE_FILES        T_("Filter.ImageFiles")
#define T_FILTER_TEXT_FILES         T_("Filter.TextFiles")
#define T_FILTER_ALL_FILES          T_("Filter.AllFiles")
#define T_IS_CONTROLLER             T_("Gamepad.IsGamepad")
#define T_CONTROLLER_ID             T_("GamepadId")
#define T_CONROLLER_L_DEADZONE      T_("Gamepad.LeftDeadZone")
#define T_CONROLLER_R_DEADZONE      T_("Gamepad.RightDeadZone")

// Lang Input History
#define S_OVERLAY_DIRECTION             "direction_up"
#define S_OVERLAY_HISTORY_SIZE          "history_size"
#define S_OVERLAY_FIX_CUTTING           "fix_cutting"
#define S_OVERLAY_INCLUDE_MOUSE	        "include_mouse"
#define S_OVERLAY_INTERVAL              "interval"
#define S_OVERLAY_CLEAR_HISTORY         "clear_history"
#define S_OVERLAY_ENABLE_REPEAT_KEYS    "repeat_keys"
#define S_OVERLAY_ENABLE_AUTO_CLEAR     "auto_clear"
#define S_OVERLAY_AUTO_CLEAR_INTERVAL   "auto_clear_interval"

#define S_OVERLAY_FONT                  "font"
#define S_OVERLAY_FONT_COLOR            "color"
#define S_OVERLAY_OUTLINE               "outline"
#define S_OVERLAY_OUTLINE_SIZE          "outline_size"
#define S_OVERLAY_OUTLINE_COLOR         "outline_color"
#define S_OVERLAY_OUTLINE_OPACITY       "outline_opacity"
#define S_OVERLAY_OPACITY               "opacity"

#define T_(v)                           obs_module_text(v)
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
#define PAD_KEY_COUNT       21
#define PAD_BODY            0
#define PAD_L_ANALOG        1
#define PAD_R_ANALOG        2
#define PAD_BACK            3
#define PAD_START           4
#define PAD_PLAYER_1        5
#define PAD_PLAYER_2        6
#define PAD_PLAYER_3        7
#define PAD_PLAYER_4        8
#define PAD_X               9
#define PAD_Y               10
#define PAD_B               11
#define PAD_A               12
#define PAD_LB              13
#define PAD_LT              14
#define PAD_RB              15
#define PAD_RT              16
#define PAD_DPAD_UP         17
#define PAD_DPAD_DOWN       18
#define PAD_DPAD_LEFT       19
#define PAD_DPAD_RIGHT      20

const char* key_to_text(int key_code);
#endif