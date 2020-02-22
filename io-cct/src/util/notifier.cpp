/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <universailp@web.de>.
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

#include "notifier.hpp"
#include "palette.hpp"
#include <algorithm>

void notifier::resize()
{
    m_dim.x = m_helper->util_window_size()->x / 2 - m_dim.w / 2;
}

void notifier::add_msg(const uint8_t type, const std::string& msg)
{
    if (!msg.empty() && (m_last_message.empty() || m_last_message != msg)) {
        m_last_message = msg;
        auto m = new Message(type, msg, m_helper);
        m_messages.emplace_back(m);
        m_dim.h += m->m_dim.h + LINE_SPACE;

        if (m->m_dim.w > m_dim.w)
            m_dim.w = m->m_dim.w + 20;
    }
#ifdef _DEBUG
    else {
        printf("Notifier error: Duplicate message or empty message\n");
    }
    printf("[Notifier] %s]: %s\n", type == MESSAGE_INFO ? " [INFO" : "[ERROR", msg.c_str());
#endif
    resize();
}

void notifier::draw()
{
    if (m_messages.empty())
        return;

    m_helper->util_fill_rect_shadow(&m_dim, m_helper->get_palette()->gray());
    uint8_t index = 0;
    auto y_pos = LINE_SPACE;
    std::vector<uint8_t> overdue;

    for (auto const& msg : m_messages) {
        if (SDL_GetTicks() - msg->m_time_stamp > (MESSAGE_TIMEOUT * msg->m_message_lines.size())) {
            overdue.emplace_back(index);
        } else {
            const auto c = msg->m_type == MESSAGE_ERROR ? m_helper->get_palette()->red()
                                                        : m_helper->get_palette()->white();

            for (auto const& line : msg->m_message_lines) {
                if (!line->empty()) {
                    m_helper->util_text(line.get(), m_dim.x + 10, m_dim.y + y_pos, c);
                }
                y_pos += LINE_SPACE + m_helper->util_default_text_height();
            }
        }
        index++;
    }

    /* Remove old messages */
    std::sort(overdue.begin(), overdue.end());
#ifdef WINDOWS
    for (auto& i = overdue.rbegin(); i != overdue.rend(); ++i)
#else
    for (auto i = overdue.rbegin(); i != overdue.rend(); ++i)
#endif
    {
        if (m_messages.at(*i)->m_message_lines[0]->compare(m_last_message) == 0) {
            m_last_message.clear();
        }
        m_dim.h -= m_helper->util_default_text_height() + LINE_SPACE * m_messages.at(*i)->m_message_lines.size();
        m_messages.erase(m_messages.begin() + *i);
    }

    if (m_messages.empty()) {
        m_dim.w = 0;
        m_dim.h = LINE_SPACE;
    }
}
