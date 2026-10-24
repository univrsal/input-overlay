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

#include <QStringList>

class input_filter {
    QStringList m_filters;
    bool m_regex = false;
    bool m_whitelist = false;
    int m_last_blocked = -1; /* Last logged blocking state, -1 = not logged yet */

public:
    ~input_filter();

    void read_from_config();

    void write_to_config();

    /* Writes the current filter setup to the log, filter_mutex has to be locked */
    void log_config();

    void set_filters(const QStringList &filters);

    void set_regex(bool enabled);

    void set_whitelist(bool wl);

    bool input_blocked();

    QStringList filters();
};
