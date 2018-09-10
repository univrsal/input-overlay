/**
 * Created by univrsal on 26.07.2017.
 * This file is part of reloded which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/reloded
 */

#include "Label.hpp"
#include "../../util/SDL_Helper.hpp"
#include "../../util/Localization.hpp"

Label::Label(const int8_t id, const int x, const int y, const char* text, Dialog* parent, uint16_t flags)
{
    const SDL_Rect temp{x, y, 0, 0}; /* Width/Height will be calculated by set_text */
    GuiElement::init(parent, temp, id);
    m_flags = flags;
    m_unlocalized_text = text;
    Label::refresh();
    m_color = get_helper()->palette()->white();
}

Label::Label(const int8_t id, const int x, const int y, const char* text, const uint8_t font, Dialog* parent, const uint16_t flags)
    : Label(id, x, y, text, parent, flags)
{
    m_font = font;
}

Label::Label(const int8_t id, const int x, const int y, const char* text, Dialog* parent, SDL_Color* color)
    : Label(id, x, y, text, parent)
{
    m_color = color;
}

Label::~Label()
{
    Label::close();
}

void Label::close()
{
    m_lines.clear();
}

void Label::draw_background()
{
    if (!m_lines.empty())
    {
        auto i = 0;
        auto y = 0;
        for (auto const& line : m_lines)
        {
            if (!line->empty())
            {
                get_helper()->util_text(line.get(), get_left(),
                                        get_top() + y,
                                        m_color, m_font);
            }
            y += LINE_SPACE + get_helper()->util_font_height(m_font);
            i++;
        }
    }
}

void Label::draw_foreground()
{
    GuiElement::draw_foreground();
}

bool Label::handle_events(SDL_Event* event, bool was_handled)
{
    return false;
}

void Label::set_text(std::string text)
{
    if (!text.empty())
    {
        m_lines.clear();
        get_helper()->format_text(&text, m_lines, m_dimensions);
    }
    else
    {
        m_lines.clear();
    }
}

void Label::refresh()
{
    if (m_flags & ELEMENT_UNLOCALIZED)
    {
        set_text(m_unlocalized_text);
    }
    else
    {
        set_text(get_helper()->loc(m_unlocalized_text.c_str()));
        m_font = get_helper()->localization()->get_font();
    }
}
