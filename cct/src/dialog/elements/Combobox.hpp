/**
 * Created by univrsal on 11.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include "GuiElement.hpp"
#include "../Dialog.hpp"

#define ITEM_V_SPACE 4

#define ACTION_COMBO_ITEM_SELECTED -15

#define ARROW_DOWN_SIZE 15

class Combobox :
	public GuiElement
{
public:
	Combobox(int8_t id, int x, int y, int w, int h, Dialog * parent, uint16_t flags = 0x0);

	void close(void) override;

	void draw_background(void) override;

	void draw_foreground(void) override;

	void add_item(std::string item) { m_items.emplace_back(item); }

	uint8_t get_selected(void) { return m_selected_id; }

	void select_item(uint8_t id) { m_selected_id = UTIL_CLAMP(0, id, m_items.size() - 1); }

	bool can_select(void) override;

	void select_state(bool state) override;

	bool handle_events(SDL_Event * event) override;

	void cycle_up(bool select);

	void cycle_down(bool select);
private:
	std::vector<std::string> m_items;
	uint8_t m_selected_id = 0;
	uint8_t m_hovered_id = 0;

	bool m_focused = false;
	bool m_list_open = false;
};
