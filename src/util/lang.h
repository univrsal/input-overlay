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

/* Misc */
#define T_(v)                           obs_module_text(v)
#define T_LOCAL_SOURCE                  T_("Source.InputSource.Local")
#define T_INPUT_SOURCE                  T_("Source.InputSource")
#define T_RELOAD_CONNECTIONS            T_("Source.InputSource.Reload")
#define T_MENU_OPEN_SETTINGS            T_("Menu.InputOverlay.OpenSettings")
#define T_REFRESH_RATE_TOOLTIP          T_("Dialog.InputOverlay.RemoteRefreshRate.Tooltip")

/* Lang Input Overlay */
#define T_TEXTURE_FILE                  T_("Overlay.Path.Texture")
#define T_LAYOUT_FILE                   T_("Overlay.Path.Layout")
#define T_LINEAR_ALPHA                  T_("Overlay.LinearAlpha")
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

/* clang-format on */
