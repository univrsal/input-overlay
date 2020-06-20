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

#include "handler.hpp"
#include "key_names.hpp"
#include <memory>
#include <QString>
#include <vector>

class input_entry;

namespace sources {
struct history_settings;
}

struct key_combination {
	/* Contructor is used by make_unique */
	explicit key_combination(QString &str)
	{
		keys = str;
		repeat = 0;
	}

	QString keys;
	uint8_t repeat;
};

class text_handler : public handler {
	std::vector<std::unique_ptr<key_combination>> m_values; /* Text body (All key combinations in order) */
	key_names m_names;                                      /* Contains custom key names */
	input_entry *m_display = nullptr;
	obs_source_t *m_text_source = nullptr;

	void make_body_text(QString &str);

public:
	explicit text_handler(sources::history_settings *settings);

	~text_handler() override;

	void load_names(const char *cfg);

	void update() override;

	void tick(float seconds) override;

	void swap(input_entry &current) override;

	void render(const gs_effect_t *effect) override;

	void clear() override;

	obs_source_t *get_text_source() const; /* Used to add the text properties in get_properties_for_history */
};
