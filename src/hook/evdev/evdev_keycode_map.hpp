/*************************************************************************
* This file is part of input-overlay
 * git.vrsal.cc/alex/input-overlay
 * based on https://codeberg.org/MaxCross/obs-input-overlay-zig
 * Copyright 2026 univrsal <uni@vrsal.cc>.
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
#include <cstdint>
#include "uiohook.h"

uint16_t evdev_to_uiohook(uint16_t evdev_code);

uint16_t evdev_mouse_to_uiohook(uint16_t evdev_code);