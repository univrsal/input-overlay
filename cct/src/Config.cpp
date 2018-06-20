#include "Config.hpp"
#include "dialog/DialogElementSettings.hpp"
#include "../../libuiohook/include/uiohook.h"

#define X_AXIS 100
#define Y_AXIS 100

Config::Config(const std::string * texture, const std::string * config, SDL_Point def_dim, SDL_helper * h, DialogElementSettings * s)
{
	m_texture_path = texture;
	m_config_path = config;
	m_settings = s;
	m_atlas = new Texture(texture->c_str(), h->renderer());
	m_helper = h;
	m_default_dim = def_dim;
	SDL_Point * w = h->util_window_size();
	m_cs = CoordinateSystem(SDL_Point { X_AXIS, Y_AXIS }, SDL_Rect { 0, 0, w->x, w->y }, h);

	if (def_dim.x > 0 && def_dim.y > 0)
		m_cs.set_grid_space(def_dim);

	SDL_Point pos = { 0, 0 };
	SDL_Rect map = { 1, 1, 157, 128 };
}

Config::~Config()
{
	if (m_atlas)
		delete m_atlas;
	m_atlas = nullptr;
	m_texture_path = nullptr;
	m_config_path = nullptr;
	m_helper = nullptr;
	m_settings = nullptr;
}

void Config::draw_elements(void)
{
	m_cs.draw_background();
	/* Draw elements */
	m_cs.begin_draw();
	std::vector<std::unique_ptr<Element>>::iterator iterator;
	for (iterator = m_elements.begin(); iterator != m_elements.end(); iterator++)
	{
		if (iterator->get() == m_selected)
			iterator->get()->draw(m_atlas, &m_cs, true);
		else
			iterator->get()->draw(m_atlas, &m_cs, false);
	}
	m_cs.end_draw();
	/* Draw coordinate system */

	m_cs.draw_foreground();

	if (m_element_to_delete >= 0 && m_element_to_delete < m_elements.size())
	{
		m_elements.erase(m_elements.begin() + m_element_to_delete);
		m_element_to_delete = -1;
	}
}

void Config::handle_events(SDL_Event * e)
{
	m_cs.handle_events(e);

	if (e->type == SDL_MOUSEBUTTONDOWN)
	{
		if (e->button.button == SDL_BUTTON_LEFT)
		{
			/* Handle selection of elements */
			std::vector<std::unique_ptr<Element>>::iterator iterator;
			int i = 0;
			for (iterator = m_elements.begin(); iterator != m_elements.end(); iterator++)
			{
				if (m_helper->util_is_in_rect(
					&iterator->get()->get_abs_dim(&m_cs),
					e->button.x, e->button.y))
				{
					m_selected = iterator->get();
					m_selected_id = i;
					m_dragging_element = true;
					m_drag_element_offset = { (int) (e->button.x - (m_selected->get_x() * m_cs.get_scale())
					+ m_cs.get_origin()->x), (int) (e->button.y - (m_selected->get_y() * m_cs.get_scale())
					+ m_cs.get_origin()->y) };

					m_settings->set_dimensions(m_selected->get_w(), m_selected->get_h());
					m_settings->set_position(m_selected->get_x(), m_selected->get_y());
					m_settings->set_uv(m_selected->get_u(), m_selected->get_v());
					m_settings->set_id(*m_selected->get_id());
					break;
				}
				i++;
			}
		}
	}
	else if (e->type == SDL_MOUSEBUTTONUP)
	{
		if (e->button.button == SDL_BUTTON_LEFT)
		{
			m_dragging_element = false;
		}
	}
	else if (e->type == SDL_MOUSEMOTION)
	{
		if (m_dragging_element && m_selected)
		{
			int x, y;
			x = SDL_max((e->button.x - m_drag_element_offset.x +
				m_cs.get_origin()->x) / m_cs.get_scale(), 0);
			y = SDL_max((e->button.y - m_drag_element_offset.y +
				m_cs.get_origin()->y) / m_cs.get_scale(), 0);

			m_selected->set_pos(x, y);
			m_settings->set_position(x, y);
		}
	}
	else if (e->type == SDL_KEYDOWN)
	{
		if (m_selected)
		/* Move selected element with arrow keys */
		{
			int x = m_selected->get_x();
			int y = m_selected->get_y();

			bool moved = false;

			switch (e->key.keysym.sym)
			{
			case SDLK_UP:
				y = SDL_max(y - 1, 0);
				moved = true;
				break;
			case SDLK_DOWN:
				y++;
				moved = true;
				break;
			case SDLK_RIGHT:
				x++;
				moved = true;
				break;
			case SDLK_LEFT:
				x = SDL_max(0, x - 1);
				moved = true;
				break;
			}

			if (moved)
			{
				m_selected->set_pos(x, y);
				m_settings->set_position(x, y);
			}
		}

		for (auto& const element : m_elements)
		{
			if (e->key.keysym.sym == vc_to_sdl_key(element->get_vc()))
			{
				element->set_pressed(true);
			}
		}
	}
	else if (e->type == SDL_KEYUP)
	{
		for (auto& const element : m_elements)
		{
			if (e->key.keysym.sym == vc_to_sdl_key(element->get_vc()))
			{
				element->set_pressed(false);
			}
		}
	}
	else if (e->type == SDL_WINDOWEVENT)
	{
		if (e->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		{
			SDL_Point * w = m_helper->util_window_size();
			m_cs.set_dimensions(SDL_Rect{ 0, 0, w->x, w->y });
		}
	}
}

Texture * Config::get_texture(void)
{
    return m_atlas;
}

SDL_Point Config::get_default_dim(void)
{
	return m_default_dim;
}

uint16_t Config::vc_to_sdl_key(uint16_t key)
{
	switch (key)
	{
		case VC_A:
			return SDLK_a;
		case VC_B:
			return SDLK_b;
		case VC_C:
			return SDLK_c;
		case VC_D:
			return SDLK_d;
		case VC_E:
			return SDLK_e;
		case VC_F:
			return SDLK_f;
		case VC_G:
			return SDLK_g;
		case VC_H:
			return SDLK_h;
		case VC_I:
			return SDLK_i;
		case VC_J:
			return SDLK_j;
		case VC_K:
			return SDLK_k;
		case VC_L:
			return SDLK_l;
		case VC_M:
			return SDLK_m;
		case VC_N:
			return SDLK_n;
		case VC_O:
			return SDLK_o;
		case VC_P:
			return SDLK_p;
		case VC_Q:
			return SDLK_q;
		case VC_R:
			return SDLK_r;
		case VC_S:
			return SDLK_s;
		case VC_T:
			return SDLK_t;
		case VC_U:
			return SDLK_u;
		case VC_V:
			return SDLK_v;
		case VC_W:
			return SDLK_w;
		case VC_X:
			return SDLK_x;
		case VC_Y:
			return SDLK_y;
		case VC_Z:
			return SDLK_z;
		case VC_0:
			return SDLK_0;
		case VC_1:
			return SDLK_1;
		case VC_2:
			return SDLK_2;
		case VC_3:
			return SDLK_3;
		case VC_4:
			return SDLK_4;
		case VC_5:
			return SDLK_5;
		case VC_6:
			return SDLK_6;
		case VC_7:
			return SDLK_7;
		case VC_8:
			return SDLK_8;
		case VC_9:
			return SDLK_9;
		case VC_SHIFT_L:
			return SDLK_LSHIFT;
		case VC_SHIFT_R:
			return SDLK_RSHIFT;
		case VC_ALT_L:
			return SDLK_LALT;
		case VC_ALT_R:
			return SDLK_RALT;
		case VC_CONTROL_L:
			return SDLK_LCTRL;
		case VC_CONTROL_R:
			return SDLK_RCTRL;
		case VC_META_L:
			return SDLK_LGUI;
		case VC_META_R:
			return SDLK_RGUI;
		case VC_SPACE:
			return SDLK_SPACE;
		case VC_BACKSPACE:
			return SDLK_BACKSPACE;
	}
}
