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

#include "input_filter.hpp"
#include "config.hpp"
#include "obs_util.hpp"
#include "log.h"
#include "settings.h"
#include <QJsonArray>
#include <QRegularExpression>
#include <util/config-file.h>

void input_filter::read_from_config()
{
    std::lock_guard lock(io_config::filter_mutex);

    m_filters.clear();
    m_regex = CGET_BOOL(S_REGEX);
    m_whitelist = CGET_INT(S_FILTER_MODE) == 0;
    QJsonDocument j;

    char *path = obs_module_config_path("filters.json");

    if (!util_open_json(utf8_to_qt(path), j)) {
        write_to_config(); // Create default file if it doesn't exist
        berr("Couldn't load filters.json from plugin config, trying legacy location");
        if (!util_open_json(util_get_data_file_legacy("filters.json"), j)) {
            berr("Couldn't load filters.json from legacy location");
            bfree(path);
            log_config();
            return;
        }
    }
    bfree(path);

    if (j.isArray()) {
        for (const auto &item : j.array()) {
            if (item.isString()) {
                m_filters.append(item.toString());
            }
        }
    }
    log_config();
}

void input_filter::log_config()
{
    m_last_blocked = -1; /* Log the blocking state again on the next check */

    if (!io_config::enable_input_control) {
        binfo("Input filtering is disabled, input will never be blocked");
        return;
    }

    binfo("Input filtering is enabled: mode: %s, regex: %s, %lli filter(s)",
          m_whitelist ? "whitelist (input only allowed for listed windows)"
                      : "blacklist (input blocked for listed windows)",
          m_regex ? "on" : "off", static_cast<long long>(m_filters.size()));

    if (m_whitelist && m_filters.empty())
        bwarn("Input filtering is in whitelist mode with no filters, ALL input will be blocked! "
              "Add windows to the list or disable input filtering in the input-overlay settings.");
}

void input_filter::write_to_config()
{

    QJsonDocument j;
    QJsonArray arr;

    for (const auto &filter : m_filters)
        arr.append(filter);
    j.setArray(arr);
    char *path = obs_module_config_path("filters.json");
    util_write_json(utf8_to_qt(path), j);

    bfree(path);
}

void input_filter::set_filters(const QStringList &filters)
{
    std::lock_guard lock(io_config::filter_mutex);
    m_filters = filters;
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

    std::lock_guard lock(io_config::filter_mutex);
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
            QRegularExpression regex(filter);
            if (regex.isValid() && regex.match(current_window.c_str()).hasMatch()) {
                flag = !m_whitelist;
                break;
            }
        }
    }

    if (m_last_blocked != static_cast<int>(flag)) {
        m_last_blocked = flag;
        if (flag)
            binfo("Input is now blocked by input filtering (%s mode)", m_whitelist ? "whitelist" : "blacklist");
        else
            binfo("Input is no longer blocked by input filtering");
    }

    return flag;
}

QStringList input_filter::filters()
{
    std::lock_guard lock(io_config::filter_mutex);
    return m_filters;
}

input_filter::~input_filter()
{
    m_filters.clear();
}
