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

#include "icon_handler.hpp"
#include "input_entry.hpp"
#include "scale_effect.hpp"
#include "sources/input_history.hpp"
#include "translate_effect.hpp"
#include <graphics/matrix4.h>
#include <layout_constants.h>
#include <util.hpp>

icon_handler::~icon_handler()
{
	clear();
}

void icon_handler::load_icons(const QString &cfg, const QString &img)
{
	m_icons.load_from_file(cfg, img);
}

void icon_handler::update()
{
	if (m_settings->icon_cfg_path && strlen(m_settings->icon_cfg_path) > 0 && m_settings->icon_path &&
		strlen(m_settings->icon_path) > 0)
		load_icons(m_settings->icon_path, m_settings->icon_cfg_path);

	switch (m_settings->dir) {
	case DIR_DOWN:
		m_translate_dir.x = 0.f;
		m_translate_dir.y = 1.f * (m_icons.get_h() + m_settings->v_space);
		m_start_pos = {};
		break;
	case DIR_UP:
		m_translate_dir.x = 0.f;
		m_translate_dir.y = -1.f * (m_icons.get_h() + m_settings->v_space);
		m_start_pos.x = 0.f;
		m_start_pos.y = static_cast<float>(m_settings->cy - m_icons.get_h());
		break;
	case DIR_LEFT:
		m_translate_dir.x = -1.f * (m_icons.get_w() + m_settings->h_space);
		m_translate_dir.y = 0.f;
		m_start_pos.x = static_cast<float>(m_settings->cx - m_icons.get_w());
		m_start_pos.y = 0.f;
		break;
	case DIR_RIGHT:
		m_start_pos = {};
		m_translate_dir.x = 1.f * (m_icons.get_w() + m_settings->h_space);
		m_translate_dir.y = 0.f;
		break;
	}
}

void icon_handler::tick(float seconds)
{
	if (m_entries.empty())
		return;

	for (auto &entry : m_entries) {
		entry->tick(seconds);
	}

	if (m_state == STATE_BLENDING && m_entries.back()->finished()) {
		m_entries.pop_back(); /* Remove finished entry */
	}
}

void icon_handler::swap(input_entry &current)
{
	if (m_state == STATE_BLENDING) { /* New input was captured before blending finished */
		/* Clear effects etc. */
		for (auto &entry : m_entries)
			entry->clear_effects();
	}

	m_state = STATE_BLENDING;
	auto new_entry = new input_entry(current);
	new_entry->set_pos(m_start_pos.x, m_start_pos.y);
	new_entry->add_effect(new scale_effect(0.5f, 1.f, 0.f));
	m_entries.push_front(std::unique_ptr<input_entry>(new_entry));

	for (auto &entry : m_entries) {
		entry->add_effect(new translate_effect(0.5f, m_translate_dir, entry->get_pos(), true));
	}
#ifdef DEBUG
	blog(LOG_DEBUG, "---");
#endif
	if (m_entries.size() > m_settings->history_size) { /* Too many entries -> last one fades out */
		m_entries.back()->add_effect(new scale_effect(0.5f, 0.f, 1.f));
		m_entries.back()->mark_for_removal();
	}
}

void icon_handler::render(const gs_effect_t *effect)
{
	if (m_entries.empty())
		return;

	gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), m_icons.image_file()->texture);
	int max_icon_count = 0;

	for (auto &entry : m_entries) {
		entry->render_icons(m_settings, &m_icons);
		max_icon_count = UTIL_MAX(max_icon_count, entry->get_input_count());
	}

	if (max_icon_count > m_old_icon_count) {
		/* Only resize if size would increase, resizing when size would decrease
		 * would result in source constantly moving around */
		switch (m_settings->dir) {
		case DIR_DOWN:
		case DIR_UP:
			m_settings->cx = max_icon_count * (m_icons.get_w() + m_settings->h_space) - m_settings->h_space;
			m_settings->cy = m_settings->history_size * (m_icons.get_w() + m_settings->v_space) - m_settings->h_space;
			break;
		case DIR_LEFT:
		case DIR_RIGHT:
			m_settings->cx = m_settings->history_size * (m_icons.get_w() + m_settings->h_space) - m_settings->h_space;
			m_settings->cy = max_icon_count * (m_icons.get_w() + m_settings->v_space) - m_settings->h_space;
			break;
		}
		m_old_icon_count = max_icon_count;
	}
}

void icon_handler::clear()
{
	m_entries.clear();
}

icon_handler::icon_handler(sources::history_settings *settings) : handler(settings) {}
