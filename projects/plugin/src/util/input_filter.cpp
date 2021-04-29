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

#include "input_filter.hpp"
#include "config.hpp"
#include "obs_util.hpp"
#include "log.h"
#include "settings.h"
#include <QJsonArray>
#include <util/config-file.h>

void input_filter::read_from_config()
{
    io_config::filter_mutex.lock();
    m_filters.clear();
    m_regex = CGET_BOOL(S_REGEX);
    m_whitelist = CGET_INT(S_FILTER_MODE) == 0;
    QJsonDocument j;
    if (!util_open_json(util_get_data_file("filters.json"), j)) {
        berr("Couldn't load filters.json");
        return;
    }

    if (j.isArray()) {
        for (const auto &item : j.array()) {
            if (item.isString()) {
                m_filters.append(item.toString());
            }
        }
    }
    io_config::filter_mutex.unlock();
}

void input_filter::write_to_config()
{
    io_config::filter_mutex.lock();

    QJsonDocument j;
    QJsonArray arr;

    for (const auto &filter : m_filters)
        arr.append(filter);
    j.setArray(arr);
    util_write_json(util_get_data_file("filters.json"), j);
    io_config::filter_mutex.unlock();
}

void input_filter::add_filter(const char *filter)
{
    io_config::filter_mutex.lock();
    m_filters.append(filter);
    io_config::filter_mutex.unlock();
}

void input_filter::remove_filter(const int index)
{
    if (index >= 0 && index < m_filters.size())
        m_filters.removeAt(index);
}

void input_filter::set_regex(bool enabled)
{
    m_regex = enabled;
}

void input_filter::set_whitelist(bool wl)
{
    m_whitelist = wl;
}

bool input_filter::input_blocked()
{
    if (!io_config::enable_input_control)
        return false;

    io_config::filter_mutex.lock();
    std::string current_window;
    auto flag = m_whitelist;
    GetCurrentWindowTitle(current_window);
    const char *window_str = current_window.c_str();

    for (const auto &filter : m_filters) {
        if (filter == window_str) {
            flag = !m_whitelist;
            break;
        }

        if (m_regex) {
            QRegExp regex(filter);
            if (regex.isValid() && regex.exactMatch(current_window.c_str())) {
                flag = !m_whitelist;
                break;
            }
        }
    }
    io_config::filter_mutex.unlock();
    return flag;
}

QStringList &input_filter::filters()
{
    return m_filters;
}

input_filter::~input_filter()
{
    m_filters.clear();
}
