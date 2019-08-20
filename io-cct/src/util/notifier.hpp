/**
 * Created by univrsal on 30.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "sdl_helper.hpp"
#include "localization.hpp"
#include <vector>
#include <memory>

#define MESSAGE_INFO  0
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
