/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2019 univrsal <universailp@web.de>.
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

#include "input_queue.hpp"
#include "sources/input_history.hpp"
#include "icon_handler.hpp"
#include "text_handler.hpp"

void input_queue::init_icon()
{
	free_handler();
	m_current_handler = new icon_handler(m_settings);
}

void input_queue::init_text()
{
	free_handler();
	m_current_handler = new text_handler(m_settings);
}

void input_queue::free_handler()
{
	m_handler_mutex.lock();
	delete m_current_handler;
	m_current_handler = nullptr;
	m_handler_mutex.unlock();
}

input_queue::input_queue(sources::history_settings *settings) : m_settings(settings)
{
	/* NO-OP */
}

input_queue::~input_queue()
{
	free_handler();
}

void input_queue::update(const sources::history_mode new_mode)
{
	if (new_mode != m_settings->mode || !m_current_handler) {
		switch (new_mode) {
		case sources::history_mode::TEXT:
			init_text();
			break;
		case sources::history_mode::ICONS:
			init_icon();
		}
	}

	m_current_handler->update();
}

obs_source_t *input_queue::get_fade_in()
{
	m_handler_mutex.lock();
	const auto h = dynamic_cast<text_handler *>(m_current_handler);
	m_handler_mutex.unlock();
	return h ? h->get_text_source() : nullptr;
}

void input_queue::collect_input()
{
	m_queued_entry.collect_inputs(m_settings);
}

void input_queue::swap()
{
	m_handler_mutex.lock();
	if (!m_queued_entry.empty() && m_current_handler) {
		m_current_handler->swap(m_queued_entry);
		m_queued_entry.clear();
	}
	m_handler_mutex.unlock();
}

void input_queue::tick(const float seconds)
{
	m_handler_mutex.lock();
	if (m_current_handler)
		m_current_handler->tick(seconds);
	m_handler_mutex.unlock();
}

void input_queue::render(gs_effect_t *effect)
{
	m_handler_mutex.lock();
	if (m_current_handler)
		m_current_handler->render(effect);
	m_handler_mutex.unlock();
}

void input_queue::clear()
{
    /* Not zero, since then the source is hard to
     * move and click on
     */
    m_handler_mutex.lock();
    if (m_current_handler)
        m_current_handler->clear();
    m_queued_entry.clear();
    m_height = 50;
    m_width = 50;
    m_handler_mutex.unlock();
}
