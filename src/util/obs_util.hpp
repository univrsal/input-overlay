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

#include <QJsonDocument>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* clang-format off */
#define GET_PROPS(S)                    (obs_properties_get(props, S))
#define GET_BOOL(X)                     (obs_data_get_bool(s, X))
/* clang-format on */

#define utf8_to_qt(_str) QString::fromUtf8(_str)
#define qt_to_utf8(_str) _str.toUtf8().constData()

/* Creates string for obs to use as accepted files for a file dialog */
QString util_file_filter(const char *display, const char *formats);

/* Changes slashes in path to fit Unix formatting */
void util_format_path(QString &path);

bool util_open_json(const QString &path, QJsonDocument &doc);

bool util_write_json(const QString &path, const QJsonDocument &doc);

/* Get file path to /home/user/.config/
 * or C:\Users\user\*
 * This is deprecated and only used to load old files
 */
QString util_get_data_file_legacy(const QString &file_name);

/* Source:
 * github.com/obsproject/obs-studio/blob/master/UI/frontend-plugins/frontend-tools/auto-scene-switcher-win.cpp
 * github.com/obsproject/obs-studio/blob/master/UI/frontend-plugins/frontend-tools/auto-scene-switcher-nix.cpp
 * github.com/obsproject/obs-studio/blob/master/UI/frontend-plugins/frontend-tools/auto-scene-switcher-osx.mm
 */
extern void GetWindowList(std::vector<std::string> &windows);

extern void GetCurrentWindowTitle(std::string &title);

#if _WIN32
extern void os_set_thread_name(const char *);
#endif
