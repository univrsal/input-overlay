#include "Button.hpp"
#include "../../util/Localization.hpp"

Button::Button(const int8_t id, const int x, const int y, const char* text, Dialog* parent)
    : Button(id, x, y, 100, text, parent)
{
    /* NO-OP*/
}

Button::Button(const int8_t id, const int x, const int y, const int w, const char* text, Dialog* parent)
{
    const SDL_Rect temp_rect = {x, y, w, 0};
    m_unlocalized_text = text;
    GuiElement::init(parent, temp_rect, id);
    Button::refresh();
}

Button::~Button()
{
    Button::close();
}

bool Button::can_select()
{
    return true;
}

void Button::resize()
{
    const auto text_dim = m_parent_dialog->helper()->util_text_dim(&m_localized_text, m_font);

    m_dimensions.h = UTIL_MAX(text_dim.h + 4, m_dimensions.h);
    m_dimensions.w = m_dimensions.w > text_dim.w + 10 ? m_dimensions.w : text_dim.w + 10;
    m_text_pos.x = m_dimensions.w / 2 - text_dim.w / 2;
    m_text_pos.y = m_dimensions.h / 2 - text_dim.h / 2;
}

void Button::refresh()
{
    if (m_flags & ELEMENT_UNLOCALIZED)
    {
        m_localized_text = m_unlocalized_text;
    }
    else
    {
        m_localized_text = get_helper()->loc(m_unlocalized_text.c_str());
    }
    resize();
}

void Button::select_state(const bool state)
{
    m_tab_focused = state;
}

void Button::draw_background()
{
    const auto color = (m_hovered || m_tab_focused)
                                  ? get_helper()->palette()->light_gray()
                                  : get_helper()->palette()->dark_gray();


    if (m_pressed)
    {
        auto dim = *get_dimensions();
        dim.x += 2;
        dim.y += 2;
        get_helper()->util_fill_rect_shadow(&dim, color, 1);
        get_helper()->util_text(&m_localized_text, dim.x + m_text_pos.x,
                                dim.y + m_text_pos.y,
                                get_helper()->palette()->white(), m_font);
    }
    else
    {
        get_helper()->util_fill_rect_shadow(get_dimensions(), color);

        get_helper()->util_text(&m_localized_text, get_dimensions()->x + m_text_pos.x,
                                get_dimensions()->y + m_text_pos.y,
                                get_helper()->palette()->white(), m_font);
    }
}


void Button::draw_foreground()
{
    GuiElement::draw_foreground();
}

bool Button::handle_events(SDL_Event* event, bool was_handled)
{
    auto handled = false;

    if (!was_handled)
    {
        if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
        {
            if (m_hovered)
            {
                m_pressed = true;
                m_parent_dialog->change_focus(m_element_id);
                handled = true;
            }
        }
        else if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT)
        {
            if (m_hovered && m_pressed)
            {
                get_parent()->action_performed(m_element_id);
                handled = true;
            }
            m_pressed = false;
        }
        else if (event->type == SDL_MOUSEMOTION)
        {
            m_hovered = is_mouse_over(event->button.x, event->button.y);
            if (event->motion.state & SDL_BUTTON_LMASK)
            {
                if (m_hovered)
                {
                    m_pressed = true;
                }
            }
        }
    }

    /* Tab is handled regardless of whether event was handled by previous element */
    if (event->type == SDL_KEYDOWN
        && m_tab_focused && event->key.keysym.sym == SDLK_RETURN)
    {
        m_pressed = true;
        handled = true;
    }
    else if (event->type == SDL_KEYUP && event->key.keysym.sym == SDLK_RETURN)
    {
        if (m_tab_focused)
        {
            get_parent()->action_performed(m_element_id);
            handled = true;
        }
        m_pressed = false;
    }
    return handled;
}

void Button::close()
{
    m_localized_text.clear();
}
