/**
 * Created by univrsal on 26.07.2017.
 * This file is part of reloded which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/reloded
 */

#include "label.hpp"
#include "../../util/sdl_helper.hpp"
#include "../../util/localization.hpp"
#include "../../util/palette.hpp"

label::label(const int8_t id, const int x, const int y, const char* text, dialog* parent, uint16_t flags)
{
    const SDL_Rect temp{x, y, 0, 0}; /* Width/Height will be calculated by set_text */
    gui_element::init(parent, temp, id);
    m_flags = flags;
    m_unlocalized_text = text;
    label::refresh();
    m_color = get_helper()->get_palette()->white();
}

label::label(const int8_t id, const int x, const int y, const char* text, const uint8_t font, dialog* parent,
             const uint16_t flags) : label(id, x, y, text, parent, flags)
{
    m_font = font;
}

label::label(const int8_t id, const int x, const int y, const char* text, dialog* parent, SDL_Color* color) : label(id,
                                                                                                                    x,
                                                                                                                    y,
                                                                                                                    text,
                                                                                                                    parent)
{
    m_color = color;
}

label::~label()
{
    label::close();
}

void label::close()
{
    m_lines.clear();
}

void label::draw_background()
{
    if (!m_lines.empty()) {
        auto i = 0;
        auto y = 0;
        for (auto const &line : m_lines) {
            if (!line->empty()) {
                get_helper()->util_text(line.get(), get_left(), get_top() + y, m_color, m_font);
            }
            y += LINE_SPACE + get_helper()->util_default_text_height();
            i++;
        }
    }
}

void label::draw_foreground()
{
    gui_element::draw_foreground();
}

bool label::handle_events(SDL_Event* event, bool was_handled)
{
    return false;
}

void label::set_text(std::string text)
{
    if (!text.empty()) {
        m_lines.clear();
        get_helper()->format_text(&text, m_lines, m_dimensions);
    } else {
        m_lines.clear();
    }
}

void label::refresh()
{
    if (m_flags & ELEMENT_UNLOCALIZED) {
        set_text(m_unlocalized_text);
    } else {
        set_text(get_helper()->loc(m_unlocalized_text.c_str()));
    }
}
