/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.xyz/alex/input-overlay
 * Copyright 2023 univrsal <uni@vrsal.xyz>.
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
/* clang-format off */

/* Settings values*/
#define S_REGION                        "input-overlay"
#define S_UIOHOOK                       "iohook"
#define S_GAMEPAD                       "gamepad"
#define S_OVERLAY                       "overlay"
#define S_HISTORY                       "history"
#define S_LOGGING                       "logging"
#define S_PORT                          "server_port"
#define S_WSS_PORT                      "wss_port"
#define S_WSS_ADDRESS                   "wss_address"
#define S_ENABLE_WSS                    "enable_wss"
#define S_REFRESH                       "server_refresh_rate"
#define S_CONTROL                       "control"
#define S_REGEX                         "regex"
#define S_FILTER_MODE                   "filter_mode"
#define S_LINEAR_ALPHA                  "linear_alpha"

/* Misc values */
#define S_INPUT_SOURCE                  "io.input_source"
#define S_RELOAD_CONNECTIONS            "io.reload_connections"

/* overlay source */
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

/* History source */
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

/* clang-format on */
