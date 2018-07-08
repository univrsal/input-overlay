#include "Button.hpp"

Button::Button(int8_t id, int x, int y, const char* text, Dialog * parent)
	: Button(id, x, y, 100, text, parent)
{
	/* NO-OP*/
}

Button::Button(int8_t id, int x, int y, int w, const char * text, Dialog * parent)
{
	SDL_Rect temp_rect;
	m_text = std::string(text);

	SDL_Rect text_dim = parent->helper()->util_text_dim(&m_text);

	temp_rect = { x, y, text_dim.w + 10, text_dim.h + 4 };
	temp_rect.w = temp_rect.w < w ? w : temp_rect.w;
	init(parent, temp_rect, id);

	m_text_pos.x = temp_rect.w / 2 - text_dim.w / 2;
	m_text_pos.y = temp_rect.h / 2 - text_dim.h / 2;
}

Button::~Button()
{
	close();
}

bool Button::can_select(void)
{
	return true;
}

void Button::resize(void)
{
	SDL_Rect text_dim = m_parent_dialog->helper()->util_text_dim(&m_text);
	m_text_pos.x = m_dimensions.w / 2 - text_dim.w / 2;
	m_text_pos.y = m_dimensions.h / 2 - text_dim.h / 2;
}

void Button::select_state(bool state)
{
	m_tab_focused = state;
}

void Button::draw_background(void)
{
	SDL_Color *color = (m_hovered || m_tab_focused)? get_helper()->palette()->light_gray() : get_helper()->palette()->dark_gray();
	
	if (m_pressed)
	{
		SDL_Rect dim = *get_dimensions();
		dim.x += 2;
		dim.y += 2;
		get_helper()->util_fill_rect_shadow(&dim, color, 1);
		get_helper()->util_text(&m_text, dim.x + m_text_pos.x,
			dim.y + m_text_pos.y,
			get_helper()->palette()->white());
	}
	else
	{
		get_helper()->util_fill_rect_shadow(get_dimensions(), color);

		get_helper()->util_text(&m_text, get_dimensions()->x + m_text_pos.x,
			get_dimensions()->y + m_text_pos.y,
			get_helper()->palette()->white());
	}
}


void Button::draw_foreground(void)
{
	GuiElement::draw_foreground();
}

bool Button::handle_events(SDL_Event *event)
{
	bool was_handled = false;

	m_hovered = is_mouse_over(event->button.x, event->button.y);

	if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
	{
		if (m_hovered)
		{
			m_pressed = true;
			was_handled = true;
		}
	}
	else if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT)
	{
		if (m_hovered)
		{
			get_parent()->action_performed(m_element_id);
			was_handled = true;
		}
		m_pressed = false;
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
	}
	else if (event->type == SDL_KEYDOWN
		&& m_tab_focused && event->key.keysym.sym == SDLK_RETURN)
	{
			m_pressed = true;
			was_handled = true;
	}
	else if (event->type == SDL_KEYUP && event->key.keysym.sym == SDLK_RETURN)
	{
		if (m_tab_focused)
		{
			get_parent()->action_performed(m_element_id);
			was_handled = true;
		}
		m_pressed = false;
	}
	return was_handled;
}

void Button::close(void)
{
	m_text.clear();
}
