#include "Notifier.hpp"

void Notifier::add_msg(uint8_t type, std::string msg)
{
#if _DEBUG
    printf("[Notifier] %s]: %s\n", type == MESSAGE_INFO ? " [INFO" : "[ERROR", msg.c_str());
#endif
    if (!msg.empty() && (m_last_message.empty() || m_last_message.compare(msg.c_str()) != 0))
    {
        m_last_message = msg;
        Message * m = new Message(type, msg, m_helper);
        m_messages.emplace_back(m);
        m_dim.y += m->m_dim.h + 8;

        if (m->m_dim.w > m_dim.x)
            m_dim.x = m->m_dim.w + 20;
    }
    else
    {
        printf("Notifier error: Duplicate message or empty message\n");
    }
}

void Notifier::draw(void)
{
    if (m_messages.empty())
        return;

    SDL_Point dim = { 0, 0 };
    SDL_Point * w = m_helper->util_window_size();

    SDL_Rect message_box;
    message_box.w = m_dim.x;
    message_box.h = m_dim.y;
    message_box.x = w->x / 2 - message_box.w / 2;
    message_box.y = 20;

    m_helper->util_fill_rect_shadow(&message_box, m_helper->palette()->gray());
    uint8_t index = 0;
    int y_pos = 4;
    std::vector<uint8_t> overdue;

    for (auto const &msg : m_messages)
    {
        if (SDL_GetTicks() - msg->m_time_stamp > (MESSAGE_TIMEOUT * msg->m_message_lines.size()))
        {
            overdue.emplace_back(index);
        }
        else
        {
            SDL_Color * c = msg->m_type == MESSAGE_ERROR ? m_helper->palette()->red()
                : m_helper->palette()->white();

            for (auto const &line : msg->m_message_lines)
            {
                if (!line.get()->empty())
                {
                    m_helper->util_text(line.get(), message_box.x + 10,
                        message_box.y + y_pos,
                        c);
                }
                y_pos += LINE_SPACE + m_helper->util_default_text_height();
            }
        }
        index++;
    }

    /* Remove old messages */
    std::sort(overdue.begin(), overdue.end());
#ifdef WINDOWS
    for (auto &i = overdue.rbegin(); i != overdue.rend(); ++i)
#else
    for (auto i = overdue.rbegin(); i != overdue.rend(); ++i)
#endif
    {
        if (m_messages.at(*i)->m_message_lines[0]->compare(m_last_message) == 0)
        {
            m_last_message.clear();
        }
        m_dim.y -= m_helper->util_default_text_height() + 8
            * m_messages.at(*i)->m_message_lines.size();
        m_messages.erase(m_messages.begin() + *i);
    }

    if (m_messages.empty())
    {
        m_dim.x = 0;
    }
}
