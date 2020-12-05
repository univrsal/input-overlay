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

#include "textbox.hpp"
#include "../../util/palette.hpp"
#include "../../util/sdl_helper.hpp"
#include <iomanip>
#include <sstream>
#include <utility>

class sdl_helper;

textbox::textbox(const int x, const int y, const int w, const int h, std::string text, dialog *parent)
{
    const SDL_Rect temp = {x, y, w, h};
    gui_element::init(parent, temp);
    set_text(std::move(text));
}

textbox::~textbox()
{
    textbox::close();
}

void textbox::close()
{
    gui_element::close();
    m_cut_text.clear();
}

void textbox::draw_foreground()
{
    gui_element::draw_foreground();
}

void textbox::draw_background()
{
    get_helper()->util_fill_rect(get_dimensions(), get_helper()->get_palette()->gray());
    if (m_flags & ELEMENT_FOCUSED) {
        get_helper()->util_draw_rect(get_dimensions(), get_helper()->get_palette()->light_gray());
    } else {
        if (m_alert) {
            get_helper()->util_draw_rect(get_dimensions(), get_helper()->get_palette()->red());
        } else {
            get_helper()->util_draw_rect(get_dimensions(), get_helper()->get_palette()->dark_gray());
        }
    }

    auto cursor_pos = get_left() + 2;

    if (!m_cut_text.empty()) {
        get_helper()->util_text(&m_cut_text, cursor_pos, get_top() + 2, get_helper()->get_palette()->white(),
                                FONT_WSTRING);
        cursor_pos += get_helper()->util_text_dim(&m_cut_text, FONT_WSTRING).w;
    }

    if (!m_composition.empty()) {
        get_helper()->util_text(&m_composition, 2 + cursor_pos, get_top() + 2, get_helper()->get_palette()->blue(),
                                FONT_WSTRING);
        cursor_pos += get_helper()->util_text_dim(&m_composition, FONT_WSTRING).w;
    }

    if (m_flags & ELEMENT_FOCUSED) {
        SDL_Rect temp = {cursor_pos, get_top() + 2, 2, get_dimensions()->h - 4};
        get_helper()->util_fill_rect(&temp, get_helper()->get_palette()->white());
    }
}

bool textbox::handle_events(SDL_Event *event, const bool was_handled)
{
    auto handled = false;
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        /* Handle focus */
        if (!was_handled) {
            sdl_helper::util_set_flag(m_flags, ELEMENT_FOCUSED, is_mouse_over());
            if (m_flags & ELEMENT_FOCUSED) {
                if (event->button.button == SDL_BUTTON_LEFT) {
                    m_alert = false;
                    SDL_SetTextInputRect(get_dimensions());
                    handled = true;
                    m_parent_dialog->change_focus(m_element_id);
                }
                if (m_flags & TEXTBOX_KEYBIND) {
                    set_hex_int(sdl_helper::sdl_key_to_vc(TO_MOUSE_MASK(event->button.button)));
                    handled = true;
                }
            } else if (m_flags & TEXTBOX_NUMERIC) {
                if (m_text.empty())
                    set_text("0");
            }
        }
    } else if (m_flags & ELEMENT_FOCUSED) {
        if (event->type == SDL_KEYDOWN) {
            if ((m_flags & TEXTBOX_KEYBIND)) {
                set_hex_int(sdl_helper::sdl_key_to_vc(event->key.keysym.sym));
            } else {
                /* Clipboard handling */
                if (event->key.keysym.sym == SDLK_v && get_helper()->is_ctrl_down() && SDL_HasClipboardText()) {
                    const auto temp = std::string(SDL_GetClipboardText());
                    const auto a = !(m_flags & TEXTBOX_NUMERIC) || is_numeric(temp);
                    const auto b = !(m_flags & TEXTBOX_HEX) || is_hex(temp);
                    const auto c = !(m_flags & TEXTBOX_NO_SPACE) || is_space_free(temp);
                    const auto d = !(m_flags & TEXTBOX_ALPHA_NUMERIC) || is_alpha_numeric(temp);

                    if (a && b && c && d) {
                        append_text(temp);
                    }
                    handled = true;
                }
                /* Deleting */
                else if (event->key.keysym.sym == SDLK_BACKSPACE) {
                    if (!m_text.empty() && m_composition.empty()) {
                        pop_back(m_text);
                        set_text(m_text);
                    }
                    handled = true;
                }
                /* IME input accepted -> clear composition */
                else if (event->key.keysym.sym == SDLK_RETURN) {
                    m_composition.clear();
                    handled = true;
                }
            }
        } else if (event->type == SDL_CONTROLLERBUTTONDOWN) {
            if ((m_flags & TEXTBOX_KEYBIND))
                set_hex_int(get_helper()->sdl_key_to_vc(TO_PAD_MASK(event->cbutton.button)));
        }
        /* Added IME input to text */
        else if (event->type == SDL_TEXTINPUT && !(m_flags & TEXTBOX_KEYBIND)) {
            const auto temp = m_text + std::string(event->text.text);
            const auto a = !(m_flags & TEXTBOX_NUMERIC) || is_numeric(temp);
            const auto b = !(m_flags & TEXTBOX_HEX) || is_hex(temp);
            const auto c = !(m_flags & TEXTBOX_NO_SPACE) || is_space_free(temp);
            const auto d = !(m_flags & TEXTBOX_ALPHA_NUMERIC) || is_alpha_numeric(temp);

            if (a && b && c && d) {
                set_text(temp);
            }
            handled = true;
        }
        /* IME composition changed */
        else if (event->type == SDL_TEXTEDITING && !(m_flags & TEXTBOX_KEYBIND)) {
            m_composition = event->edit.text;
            handled = true;
        }
    }

    if (!was_handled && m_flags & TEXTBOX_DROP_FILE) {
        if (event->type == SDL_DROPFILE && is_mouse_over()) {
            const auto dropped_file = event->drop.file;
            if (dropped_file)
                set_text(std::string(dropped_file));
            SDL_free(dropped_file);
            m_parent_dialog->action_performed(ACTION_FILE_DROPPED);
            handled = true;
        }
    }

    if (handled)
        m_parent_dialog->action_performed(ACTION_TEXT_TYPED);
    return handled;
}

bool textbox::can_select()
{
    return true;
}

void textbox::select_state(const bool state)
{
    sdl_helper::util_set_flag(m_flags, ELEMENT_FOCUSED, state);
    if (!state)
        get_text(); /* Reset if empty */
}

void textbox::set_text(int i)
{
    set_text(std::to_string(i));
}

void textbox::set_text(double d)
{
    set_text(std::to_string(d));
}

void textbox::set_text(float f)
{
    set_text(std::to_string(f));
}

void textbox::set_text(std::string s)
{
    m_text = std::move(s);
    if (m_flags & TEXTBOX_NUMERIC || m_flags & TEXTBOX_HEX) {
        m_text = m_text.substr(0, 6); /* 5 digits is more than enough */
    }
    m_cut_text = m_text;

    /*
        We have to leave space for the composition
        which is the currently written text through
        the IME (Only for Japanese, Chinese etc.)
    */

    get_helper()->util_cut_string(m_cut_text.append(m_composition), get_dimensions()->w - m_cut_off, false);
}

void textbox::set_hex_int(const uint16_t i)
{
    std::stringstream stream;
    stream << "0x" << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << std::hex << i;
    set_text(stream.str());
}

void textbox::append_text(const std::string &s)
{
    set_text(m_text + s);
}

uint8_t textbox::get_cursor()
{
    return CURSOR_I_BEAM;
}

const std::string *textbox::get_text()
{
    if (m_flags & TEXTBOX_NUMERIC && m_text.empty())
        set_text("0");
    if (m_flags & TEXTBOX_HEX && m_text.empty())
        set_text("0x0");
    return &m_text;
}

const char *textbox::c_str()
{
    return get_text()->c_str();
}

inline bool textbox::is_numeric(const std::string &s)
{
    auto index = 0;
    for (auto i : s) {
        if (index == 0 && i == '-')
            continue;
        if (!(i >= '0' && i <= '9'))
            return false;
        index++;
    }
    return true;
}

inline bool textbox::is_alpha_numeric(const std::string &s)
{
    const auto c = s.c_str();
    return c[strspn(c, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-.:;,0123456789")] == 0;
}

inline bool textbox::is_space_free(const std::string &s)
{
    return s.find(' ') == std::string::npos;
}

bool textbox::is_hex(const std::string &s)
{
    const auto c = s.c_str();
    return c[strspn(c, "0123456789xabcdefABCDEF")] == 0;
}

void textbox::pop_back(std::string &s)
{
    auto convert = sdl_helper::util_utf8_to_wstring(s);
    convert.pop_back();
    s = sdl_helper::util_wstring_to_utf8(convert);
}

void textbox::set_alert(const bool state)
{
    m_alert = state;
}

void textbox::set_cutoff(const uint8_t c)
{
    m_cut_off = c;
}
