#include "Config.hpp"

#define X_AXIS 90
#define Y_AXIS 70

Config::Config(const std::string * texture, const std::string * config, SDL_helper * h, DialogElementSettings * s)
{
	m_texture_path = texture;
	m_config_path = config;
	m_settings = s;
	m_atlas = new Texture(texture->c_str(), h->renderer());
	m_helper = h;

	SDL_Point * w = h->util_window_size();
	m_cs = CoordinateSystem(SDL_Point { X_AXIS, Y_AXIS }, SDL_Rect { 0, 0, w->x, w->y }, h);

	SDL_Point pos = { 0, 0 };
	SDL_Rect map = { 1, 1, 157, 128 };
	m_elements.emplace_back(new Element(BUTTON_KEYBOARD, pos, map, 0x0));
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
	/* Draw elements */
	std::vector<std::unique_ptr<Element>>::iterator iterator;
	for (iterator = m_elements.begin(); iterator != m_elements.end(); iterator++)
	{
		if (iterator->get() == m_selected)
			iterator->get()->draw(m_atlas, &m_cs, true);
		else
			iterator->get()->draw(m_atlas, &m_cs, false);
	}

	/* Draw coordinate system */
	m_cs.draw_background();
	m_cs.draw_foreground();
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

			for (iterator = m_elements.begin(); iterator != m_elements.end(); iterator++)
			{
				if (m_helper->util_is_in_rect(
					&iterator->get()->get_abs_dim(&m_cs),
					e->button.x, e->button.y))
				{
					m_selected = iterator->get();
					m_dragging_element = true;
					m_drag_element_offset = { e->button.x - (m_selected->get_x() * m_cs.get_scale())
					+ m_cs.get_origin()->x, e->button.y - (m_selected->get_y() * m_cs.get_scale())
					+ m_cs.get_origin()->y };

					m_settings->set_dimensions(m_selected->get_w(), m_selected->get_h());
					m_settings->set_position(m_selected->get_x(), m_selected->get_y());
					m_settings->set_uv(m_selected->get_u(), m_selected->get_v());
					break;
				}
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
	else if (e->type == SDL_KEYDOWN && m_selected)
	{
		int x = m_selected->get_x();
		int y = m_selected->get_y();

		switch (e->key.keysym.sym)
		{
		case SDLK_UP:
			y = SDL_max(y - 1, 0);
			break;
		case SDLK_DOWN:
			y++;
			break;
		case SDLK_RIGHT:
			x++;
			break;
		case SDLK_LEFT:
			x = SDL_max(0, x - 1);
			break;
		}

		m_selected->set_pos(x, y);
		m_settings->set_position(x, y);
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
