#include "Button.hpp"

Button::Button(int8_t id, int x, int y, const char* text, Dialog * parent)
{
	SDL_Rect temp_rect;
	m_pressed = false;
	m_tooltip_shown = false;
	m_hovered = false;
	m_hover_start = 0;
	m_text = std::string(text);
	SDL_Rect text_dim = parent->helper()->util_text_dim(&m_text);
	temp_rect = { x, y, text_dim.w + 10, text_dim.h + 4 };
	temp_rect.w = temp_rect.w < 100 ? 100 : temp_rect.w;
	m_text_pos.x = temp_rect.w / 2 - text_dim.w / 2;
	m_text_pos.y = temp_rect.h / 2 - text_dim.h / 2;
	init(parent, temp_rect, id);
}

Button::~Button()
{
	close();
}

void Button::draw_background(void)
{
	SDL_Color *color = m_hovered ? get_helper()->palette()->light_gray() : get_helper()->palette()->dark_gray();
	get_helper()->util_fill_rect_shadow(get_dimensions(), color);

	get_helper()->util_text(&m_text, get_dimensions()->x + m_text_pos.x,
		get_dimensions()->y + m_text_pos.y,
		get_helper()->palette()->white());
}


void Button::draw_foreground(void)
{
	GuiElement::draw_foreground();
}

bool Button::handle_events(SDL_Event *event)
{
	bool was_handled = false;

	m_hovered = is_mouse_over(event->button.x, event->button.y);

	if (event->type == SDL_MOUSEBUTTONDOWN)
	{
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			if (m_hovered)
			{
				m_pressed = true;
				was_handled = true;
			}
		}
	}
	else if (event->type == SDL_MOUSEBUTTONUP)
	{
		if (event->button.button == SDL_BUTTON_LEFT)
		{

			if (m_hovered)
			{
				get_parent()->action_performed(m_element_id);
				was_handled = true;
			}
			m_pressed = false;
		}
	}
	else if (event->type == SDL_MOUSEMOTION)
	{
		if (event->motion.state & SDL_BUTTON_LMASK)
		{
			if (m_hovered)
			{
				m_pressed = true;
			}
		}
		else
		{
			m_pressed = false;
		}
	}

	return was_handled;
}

void Button::close(void)
{
	m_text.clear();
	m_hover_start = 0;
}
