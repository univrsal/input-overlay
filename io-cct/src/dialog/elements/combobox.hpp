/**
 * Created by univrsal on 11.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "gui_element.hpp"
#include "../dialog.hpp"

#define ITEM_V_SPACE 4

class combobox : public gui_element
{
public:
    combobox(int8_t id, int x, int y, int w, int h, dialog* parent, uint16_t flags = 0x0);

    void close() override;

    void draw_background() override;

    void draw_foreground() override;

    void add_item(std::string item)
    {
        if (m_flags & ELEMENT_UNLOCALIZED)
            m_items.emplace_back(item);
        else
            m_items.emplace_back(get_helper()->loc(item.c_str()));

        m_item_box = {get_left(), get_bottom() - 1, get_width(),
                      static_cast<int>(m_items.size() * m_item_v_space + ITEM_V_SPACE)};
    };

    uint8_t get_selected() const
    { return m_selected_id; }

    void select_item(const uint8_t id)
    {
        m_selected_id = UTIL_CLAMP(0, id, m_items.size() - 1);
        m_parent_dialog->action_performed(ACTION_COMBO_ITEM_SELECTED);
    }

    void toggle_list();

    bool can_select() override;

    void select_state(bool state) override;

    bool handle_events(SDL_Event* event, bool was_handled) override;

    bool is_mouse_over_list(const SDL_Point* p = nullptr) const;

    bool is_mouse_over(const SDL_Point* p = nullptr) override;

    void cycle_up(bool select);

    void cycle_down(bool select);

    void resize() override;

private:
    std::vector<std::string> m_items;
    SDL_Rect m_item_box;
    uint8_t m_font = FONT_WSTRING;
    uint8_t m_selected_id = 0;
    uint8_t m_hovered_id = 0;
    uint8_t m_item_v_space = 0;

    bool m_focused = false;
    bool m_list_open = false;
};
