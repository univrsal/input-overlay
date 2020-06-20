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

#include "key_names.hpp"
#include "../obs_util.hpp"
#include <obs-module.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

void key_names::load_from_file(const QString &path)
{
	m_names.clear();
	QJsonDocument doc;
	if (util_open_json(path, doc)) {
		auto cfg_arr = doc.array();
		for (const auto item : cfg_arr) {
			auto obj = item.toObject();
			if (obj["debug"].isNull()) {
				uint16_t vc = obj["code"].toInt();
				m_names[vc] = obj["name"].toString();
			} else {
				m_debug = obj["debug"].toBool();
			}
		}
	}
}

bool key_names::debug() const
{
	return m_debug;
}

bool key_names::empty() const
{
	return m_names.empty();
}

const QString &key_names::get_name(const uint16_t vc)
{
	/* Users can use an empty name in the config to
     * prevent certain keys from showing up in input-history
     * empty name = key is disabled
     */
	if (m_names.find(vc) != m_names.end() && !m_names[vc].isEmpty())
		return m_names[vc];

	return QString();
}
