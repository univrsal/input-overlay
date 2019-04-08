/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <QtCore/QStringList>

typedef struct config_data config_t;

class input_filter
{
    QStringList m_filters;
    bool m_regex = false;
    bool m_whitelist = false;
public:
    ~input_filter();

    void read_from_config(config_t* cfg);

    void write_to_config(config_t* cfg);

    void add_filter(const char* filter);

    void remove_filter(int index);

    void set_regex(bool enabled);

    void set_whitelist(bool wl);

    bool input_blocked();

    QStringList &filters();
};