/**
 * Created by universal on 8/1/17.
 * This file is part of reloded which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/reloded
 */


#include "Textbox.hpp"
#include "../../util/SDL_helper.hpp"

class SDL_helper;

Textbox::Textbox(int8_t id, int x, int y, int w, int h, std::string text, Dialog *parent)
{
	SDL_Rect temp = { x, y, w, h };
	init(parent, temp, id);
	set_text(text);
	m_focused = false;
}

Textbox::~Textbox()
{
	close();
}

void Textbox::close(void)
{
	GuiElement::close();
	m_cut_text.clear();
	m_focused = false;
}

void Textbox::draw_foreground(void)
{
	GuiElement::draw_foreground();
	int cursor_pos = get_left() + 2;

	if (!m_cut_text.empty())
	{
		cursor_pos += get_helper()->util_text_utf8_dim(&m_cut_text).w;
		get_helper()->util_text_utf8(&m_cut_text, get_left() + 2, get_top() + 2,
			get_helper()->palette()->white());
	}

	if (!m_composition.empty())
	{
		SDL_Rect temp = get_helper()->util_text_utf8_dim(&m_cut_text);
		get_helper()->util_text_utf8(&m_composition, get_left() + 2 + temp.w, get_top() + 2,
			get_helper()->palette()->blue());
		cursor_pos += temp.w;
	}

	if (m_focused)
	{
		SDL_Rect temp = { cursor_pos, get_top() + 2, 2, get_dimensions()->h - 4 };
		get_helper()->util_fill_rect(&temp, get_helper()->palette()->white());
	}
}

void Textbox::draw_background(void)
{
	get_helper()->util_fill_rect(get_dimensions(), get_helper()->palette()->gray());
	if (m_focused)
	{
		get_helper()->util_draw_rect(get_dimensions(), get_helper()->palette()->light_gray());
	}
	else
	{
		if (m_alert)
		{
			get_helper()->util_draw_rect(get_dimensions(), get_helper()->palette()->red());
		}
		else
		{
			get_helper()->util_draw_rect(get_dimensions(), get_helper()->palette()->dark_gray());
		}
	}
}

bool Textbox::handle_events(SDL_Event * event)
{
	bool was_handled = false;
	if (event->type == SDL_MOUSEBUTTONDOWN)
	{
		/* Handle focus */
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			m_focused = is_mouse_over(event->button.x, event->button.y);
			if (m_focused)
			{
				m_alert = false;
				SDL_SetTextInputRect(get_dimensions());
				was_handled = true;
			}
			else if (m_flags & TEXTBOX_NUMERIC)
			{
				if (m_text.empty())
					set_text("0");
			}
		}
	}
	else if (m_focused)
	{
		if (event->type == SDL_KEYDOWN)
		{
			/* Clipboard handling */
			if (event->key.keysym.sym == SDLK_v && get_helper()->is_ctrl_down())
			{
				if (SDL_HasClipboardText())
				{
					std::string temp = std::string(SDL_GetClipboardText());

					if (!(m_flags & TEXTBOX_NUMERIC) || is_numeric(temp))
					{
						append_text(temp);
					}
					was_handled = true;
				}
			}
			/* Deleting */
			else if (event->key.keysym.sym == SDLK_BACKSPACE)
			{
				if (!m_text.empty())
				{
					m_text.pop_back();
					set_text(m_text);
					was_handled = true;
				}
			}
			/* IME input accepted -> clear composition */
			else if (event->key.keysym.sym == SDLK_RETURN)
			{
				m_composition.clear();
				was_handled = true;
			}
		}
		/* Added IME input to text */
		else if (event->type == SDL_TEXTINPUT)
		{
			std::string temp = std::string(event->text.text);
			if (!(m_flags & TEXTBOX_NUMERIC) || is_numeric(temp))
			{
				append_text(temp);
				was_handled = true;
			}
		}
		/* IME composition changed */
		else if (event->type == SDL_TEXTEDITING)
		{
			m_composition = event->edit.text;
			was_handled = true;
		}
	}

	return was_handled;
}

void Textbox::set_text(std::string s)
{
	m_text = s;
	if (m_flags & TEXTBOX_NUMERIC)
	{
		m_text = m_text.substr(0, 5); /* 5 digits is more than enough */
	}
	m_cut_text = m_text;

	/*
		We have to leave space for the composition
		which is the currently written text through
		the IME (Only for Japanese, Chinese etc.)
	*/

	get_helper()->util_cut_string(m_cut_text.append(m_composition), get_dimensions()->w - m_cut_off, false);
}

void Textbox::append_text(std::string s)
{
	set_text(m_text.append(s));
}

const std::string * Textbox::get_text()
{
	if (m_flags & TEXTBOX_NUMERIC && m_text.empty())
		set_text("0");
	return &m_text;
}

inline bool Textbox::is_numeric(const std::string & s)
{
	for (int i = 0; i < s.length(); i++)
	{
		if (!(s[i] >= '0' && s[i] <= '9')) return false;
	}
	return true;
}

void Textbox::set_alert(bool state)
{
	m_alert = state;
}

void Textbox::set_cutoff(uint8_t c)
{
	m_cut_off = c;
}
