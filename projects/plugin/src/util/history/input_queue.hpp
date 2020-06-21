/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "input_entry.hpp"
#include "../../sources/input_history.hpp"
#include <mutex>
class handler;

class input_queue {
	std::mutex m_handler_mutex; /* Prevents deletion of handlers while rendering */
	sources::history_settings *m_settings{};
	uint16_t m_width = 0, m_height = 0;

	input_entry m_queued_entry;
	handler *m_current_handler = nullptr;

	/* Prepare/free the respective display modes */
	void init_icon();

	void init_text();

	void free_handler();

public:
	explicit input_queue(sources::history_settings *settings);

	~input_queue();

	/* Any of the three sources could be used, since this is only used to get the
     * text source properties */
	obs_source_t *get_fade_in();

	void collect_input(); /* Accumulates input events in current entry */
	void swap();          /* Adds current entry to the list */
	void tick(float seconds);

	void update(sources::history_mode new_mode);

	void render(gs_effect_t *effect);

	void clear();
};
