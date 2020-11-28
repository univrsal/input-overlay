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

#pragma once

#include "localization.hpp"
#include "sdl_helper.hpp"
#include <memory>
#include <vector>

#define MESSAGE_INFO 0
#define MESSAGE_ERROR 1

#define MESSAGE_TIMEOUT 4000

class sdl_helper;

struct Message {
public:
    Message(const uint8_t type, const std::string &msg, sdl_helper *h)
    {
        h->format_text(&msg, m_message_lines, m_dim);
        m_type = type;
        m_time_stamp = SDL_GetTicks();
    }

    ~Message()
    {
        m_message_lines.clear();
        m_type = 0;
        m_time_stamp = 0;
    }

    std::vector<std::unique_ptr<std::string>> m_message_lines;
    SDL_Rect m_dim;
    uint8_t m_type;
    uint32_t m_time_stamp;
};

class notifier {
public:
    notifier(sdl_helper *h)
    {
        m_messages = std::vector<std::unique_ptr<Message>>();
        m_helper = h;
        m_dim = {0, 20, 0, LINE_SPACE};
    }

    ~notifier()
    {
        m_messages.clear();
        m_helper = nullptr;
        m_dim = {0, 0};
    }

    void resize();

    void add_msg(uint8_t type, const std::string &msg);

    void draw();

private:
    std::string m_last_message = "";
    std::vector<std::unique_ptr<Message>> m_messages;
    sdl_helper *m_helper;
    SDL_Rect m_dim;
};
