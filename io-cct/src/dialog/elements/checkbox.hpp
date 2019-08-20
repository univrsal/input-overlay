/**
 * Created by univrsal on 25.06.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "gui_element.hpp"
#include "../dialog.hpp"

#define CHECKBOX_SIZE 17

class checkbox : public gui_element {
public:

	checkbox(int8_t id, int x, int y, const char *text, dialog *parent);

	checkbox(int8_t id, int x, int y, const char *text, dialog *parent, bool state);

	~checkbox();

	void close() override;

	bool can_select() override;

	void select_state(bool state) override;

	void draw_background() override;

	void draw_foreground() override;

	bool handle_events(SDL_Event *event, bool was_handled) override;

	bool get_state() const
	{
		return m_state;
	}

	void set_checked(bool state);

private:
	bool m_focused = false;
	bool m_state = false;
	label *m_label = nullptr;
	SDL_Rect m_checkbox;
};
