/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "input_filter.hpp"
#include "util.hpp"
#include "config.hpp"
#include <util/config-file.h>

static std::string base_id = S_FILTER_BASE;

void input_filter::read_from_config(config_t* cfg)
{
    io_config::filter_mutex.lock();
    m_regex = config_get_bool(cfg, S_REGION, S_REGEX);
    m_whitelist = config_get_int(cfg, S_REGION, S_FILTER_MODE) == 0;
    const int filter_count = config_get_int(cfg, S_REGION, S_FILTER_COUNT);

    for (auto i = 0; i < filter_count; ++i) {
        const auto str = config_get_string(cfg, S_REGION, (base_id + std::to_string(i)).c_str());
        if (str && strlen(str) > 0)
            m_filters.append(str);
    }
    io_config::filter_mutex.unlock();
}

void input_filter::write_to_config(config_t* cfg)
{
    io_config::filter_mutex.lock();

    config_set_int(cfg, S_REGION, S_FILTER_COUNT, m_filters.size());

    for (auto i = 0; i < m_filters.size(); i++) {
        config_set_string(cfg, S_REGION, (base_id + std::to_string(i)).c_str(), m_filters[i].toStdString().c_str());
    }

    io_config::filter_mutex.unlock();
}

void input_filter::add_filter(const char* filter)
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
    io_config::filter_mutex.lock();
    std::string current_window;
    auto flag = m_whitelist;
    GetCurrentWindowTitle(current_window);
    const char* window_str = current_window.c_str();

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
