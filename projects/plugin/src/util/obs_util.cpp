/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2021 univrsal <uni@vrsal.de>.
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

#include "obs_util.hpp"
#include "log.h"
#include "lang.h"
#include <QFile>
#include <QDir>

QString util_get_data_file(const QString &file_name)
{
    QDir home = QDir::homePath();
#if UNIX
    if (!home.mkpath(".config/input-ovleray")) {
        berr("Couldn't create .config/input-overlay directory!");
    }
    home.cd(".config/input-overlay");
#else
    if (!home.mkpath("input-overlay")) {
        berr("Couldn't create ~/input-overlay directory!");
    }
    home.cd("input-overlay");
#endif
    return QDir::toNativeSeparators(home.absoluteFilePath(file_name));
}

QString util_file_filter(const char *display, const char *formats)
{
    QString filter = display;
    filter += " (";
    filter += formats;
    filter += ");;";
    filter += T_FILTER_ALL_FILES;
    filter += " (*.*)";

    return filter;
}

void util_format_path(QString &path)
{
    path.replace('\\', '/');
    if (path.endsWith('/'))
        path.chop(1);
}

bool util_open_json(const QString &path, QJsonDocument &doc)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
        berr("couldn't open %s", qt_to_utf8(path));
        return false;
    }

    QJsonParseError err;
    bool result = true;
    doc = QJsonDocument::fromJson(file.readAll(), &err);

    if (err.error != QJsonParseError::NoError) {
        result = false;
        berr("Json parse error for %s: %s", qt_to_utf8(path), qt_to_utf8(err.errorString()));
    }
    return result;
}

bool util_write_json(const QString &path, const QJsonDocument &doc)
{
    QFile file(path);

    if (!file.open(QIODevice::WriteOnly)) {
        berr("couldn't open %s", qt_to_utf8(path));
        return false;
    }

    bool result = true;
    auto data = doc.toJson(QJsonDocument::Indented);
    auto wrote = file.write(data);
    if (wrote != data.length()) {
        berr("Only write %llx out of %i bytes", wrote, data.length());
        result = false;
    }
    return result;
}
