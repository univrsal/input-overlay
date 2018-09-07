/**
 * Created by univrsal on on 11.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "Combobox.hpp"
#include "../../util/Localization.hpp"

Combobox::Combobox(int8_t id, int x, int y, int w, int h, Dialog * parent, uint16_t flags)
{
    SDL_Rect temp = { x, y, w, h };
    m_flags = flags;
    init(parent, temp, id);
    m_item_v_space = get_helper()->util_font_height(FONT_WSTRING) + ITEM_V_SPACE;
}

void Combobox::close(void)
{
    m_items.clear();
}

static std::string ARROW_DOWN = "▼";

void Combobox::draw_background(void)
{
    get_helper()->util_fill_rect(get_dimensions(), get_helper()->palette()->gray());
    get_helper()->util_text(&ARROW_DOWN, get_right() - 18,
        get_top() + 2, get_helper()->palette()->white(), FONT_WSTRING);

    if (m_focused)
    {
        get_helper()->util_draw_rect(get_dimensions(), get_helper()->palette()->light_gray());
    }
    else
    {
        get_helper()->util_draw_rect(get_dimensions(), get_helper()->palette()->dark_gray());
    }
}

void Combobox::draw_foreground(void)
{
    GuiElement::draw_foreground();

    if (!m_items.empty() && m_selected_id >= 0 && m_selected_id < m_items.size())
        get_helper()->util_text(&m_items[m_selected_id], get_left() + 2, get_top() + 2, get_helper()->palette()->white(),
        m_flags & ELEMENT_UNLOCALIZED ? FONT_WSTRING : get_helper()->localization()->get_font());

    if (m_list_open)
    {
        uint16_t y = get_bottom() + ITEM_V_SPACE;
        SDL_Rect temp = { get_left(), get_bottom() - 1, get_width(),
            ((int)m_items.size()) * m_item_v_space + ITEM_V_SPACE };

        get_helper()->util_fill_rect(&temp, get_helper()->palette()->gray());
        get_helper()->util_draw_rect(&temp, get_helper()->palette()->light_gray());

        temp.y = get_bottom() + ITEM_V_SPACE + m_hovered_id * m_item_v_space;
        temp.h = get_helper()->util_font_height(FONT_WSTRING);
        get_helper()->util_fill_rect(&temp, get_helper()->palette()->light_gray());

        for (auto const& element : m_items)
        {
            get_helper()->util_text(&element, get_left() + 2, y, get_helper()->palette()->white(),
                m_flags & ELEMENT_UNLOCALIZED ? FONT_WSTRING : get_helper()->localization()->get_font());
            y += m_item_v_space;
        }
    }
}

bool Combobox::can_select(void)
{
    return true;
}

void Combobox::select_state(bool state)
{
    m_focused = state;
}

bool Combobox::handle_events(SDL_Event * event, bool was_handled)
{
    bool handled = false;
    if (event->type == SDL_MOUSEBUTTONDOWN && !was_handled)
    {
        /* Handle focus */
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            m_focused = is_mouse_over(event->button.x, event->button.y);
            if (is_mouse_over_list(event->button.x, event->button.y))
            {
                select_item(m_hovered_id);
                handled = true;
            }
                
            if (m_focused)
            {
                handled = true;
                m_list_open = !m_list_open;
                m_parent_dialog->change_focus(m_element_id);
                m_hovered_id = 0;
            }
            else
            {
                m_list_open = false;
            }
        }
    }
    else if ((m_list_open || m_focused) && event->type == SDL_MOUSEWHEEL)
    {
        
        SDL_Point mouse_pos = { 0, 0 };
        SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
        if (mouse_pos.x >= get_left() && mouse_pos.x <= get_right())
        {
            if (event->wheel.y > 0)
                cycle_up(false);
            else
                cycle_down(false);

            if (m_list_open)
            {
                // Move mouse to item position (Little gimmick, but I like the feature)
                mouse_pos.y = get_bottom() + m_hovered_id * m_item_v_space + ITEM_V_SPACE * 1.5;
                SDL_WarpMouseInWindow(nullptr, mouse_pos.x, mouse_pos.y);
            }
        }
    }
    else if (m_list_open && event->type == SDL_MOUSEMOTION)
    {
        int m_x = event->button.x - get_left();
        int m_y = event->button.y - get_bottom();

        if (m_x >= 0 && m_x <= m_dimensions.w && m_y >= 0)
        {
            m_hovered_id = UTIL_MIN(m_y / m_item_v_space, m_items.size() - 1);
        }
    }
    else if (event->type == SDL_KEYDOWN)
    {
        if (m_focused && event->key.keysym.sym == SDLK_RETURN)
        {
            if (m_list_open)
                select_item(m_hovered_id);

            m_list_open = !m_list_open;
            m_hovered_id = 0;
        }
        else if (m_list_open || m_focused)
        {
            if (event->key.keysym.sym == SDLK_UP)
                cycle_up(!m_list_open);
            else if (event->key.keysym.sym == SDLK_DOWN)
                cycle_down(!m_list_open);
        }
    }

    return handled;
}

bool Combobox::is_mouse_over_list(const int & x, const int & y)
{
    return m_list_open && get_helper()->util_is_in_rect(&m_item_box, x, y);
}

bool Combobox::is_mouse_over(const int &x, const int &y)
{
    return GuiElement::is_mouse_over(x, y) || is_mouse_over_list(x, y);
}

void Combobox::cycle_up(bool select)
{
    if (m_hovered_id - 1 < 0)
        m_hovered_id = m_items.size() - 1;
    else
        m_hovered_id--;
    if (select)
        select_item(m_hovered_id);
}

void Combobox::cycle_down(bool select)
{
    if (m_hovered_id + 1 >= m_items.size())
        m_hovered_id = 0;
    else
        m_hovered_id++;
    if (select)
        select_item(m_hovered_id);
}
