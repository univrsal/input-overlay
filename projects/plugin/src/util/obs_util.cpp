/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
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
