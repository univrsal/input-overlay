/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2021 univrsal <uni@vrsal.de>.
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

#include "button.hpp"
#include "../../util/localization.hpp"
#include "../../util/palette.hpp"

button::button(const int x, const int y, const char *text, dialog *parent) : button(x, y, 100, text, parent)
{
    /* NO-OP*/
}

button::button(const int x, const int y, const int w, const char *text, dialog *parent)
{
    const SDL_Rect temp_rect = {x, y, w, 0};
    m_unlocalized_text = text;
    gui_element::init(parent, temp_rect);
    button::refresh();
}

button::~button()
{
    button::close();
}

bool button::can_select()
{
    return true;
}

void button::resize()
{
    const auto text_dim = m_parent_dialog->helper()->util_text_dim(&m_localized_text, m_font);

    m_dimensions.h = UTIL_MAX(text_dim.h + 4, m_dimensions.h);
    m_dimensions.w = m_dimensions.w > text_dim.w + 10 ? m_dimensions.w : text_dim.w + 10;
    m_text_pos.x = m_dimensions.w / 2 - text_dim.w / 2;
    m_text_pos.y = m_dimensions.h / 2 - text_dim.h / 2;
}

void button::refresh()
{
    if (m_flags & ELEMENT_UNLOCALIZED) {
        m_localized_text = m_unlocalized_text;
    } else {
        m_localized_text = get_helper()->loc(m_unlocalized_text.c_str());
    }
    resize();
}

void button::select_state(const bool state)
{
    m_tab_focused = state;
}

void button::draw_background()
{
    const auto color = (m_hovered || m_tab_focused) ? get_helper()->get_palette()->light_gray()
                                                    : get_helper()->get_palette()->dark_gray();

    if (m_pressed) {
        auto dim = *get_dimensions();
        dim.x += 2;
        dim.y += 2;
        get_helper()->util_fill_rect_shadow(&dim, color, 1);
        get_helper()->util_text(&m_localized_text, dim.x + m_text_pos.x, dim.y + m_text_pos.y,
                                get_helper()->get_palette()->white(), m_font);
    } else {
        get_helper()->util_fill_rect_shadow(get_dimensions(), color);

        get_helper()->util_text(&m_localized_text, get_dimensions()->x + m_text_pos.x,
                                get_dimensions()->y + m_text_pos.y, get_helper()->get_palette()->white(), m_font);
    }
}

void button::draw_foreground()
{
    gui_element::draw_foreground();
}

bool button::handle_events(SDL_Event *event, bool was_handled)
{
    auto handled = false;

    if (!was_handled) {
        if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
            if (m_hovered) {
                m_pressed = true;
                m_parent_dialog->change_focus(m_element_id);
                handled = true;
            }
        } else if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
            if (m_hovered && m_pressed) {
                get_parent()->action_performed(m_element_id);
                handled = true;
            }
            m_pressed = false;
        } else if (event->type == SDL_MOUSEMOTION) {
            m_hovered = is_mouse_over();
            if (event->motion.state & SDL_BUTTON_LMASK) {
                if (m_hovered) {
                    m_pressed = true;
                }
            }
        }
    }

    /* Tab is handled regardless of whether event was handled by previous element */
    if (event->type == SDL_KEYDOWN && m_tab_focused && event->key.keysym.sym == SDLK_RETURN) {
        m_pressed = true;
        handled = true;
    } else if (event->type == SDL_KEYUP && event->key.keysym.sym == SDLK_RETURN) {
        if (m_tab_focused) {
            get_parent()->action_performed(m_element_id);
            handled = true;
        }
        m_pressed = false;
    }
    return handled;
}

void button::close()
{
    m_localized_text.clear();
}
