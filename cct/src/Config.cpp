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
	SDL_Point * window = m_helper->util_window_size();

	/* Draw elements */
	std::vector<std::unique_ptr<Element>>::iterator iterator;
	SDL_Rect bounds = { X_AXIS, Y_AXIS, window->x - X_AXIS - 20, window->y - Y_AXIS - 40 };

	for (iterator = m_elements.begin(); iterator != m_elements.end(); iterator++)
	{
		if (iterator->get() == m_selected)
			iterator->get()->draw(m_atlas, &m_cs, true);
		else
			iterator->get()->draw(m_atlas, &m_cs, false);
	}

	/* Draw Scale*/
	int step = 10 * m_cs.get_scale();;
	int start;

	m_cs.draw_background();
	m_cs.draw_foreground();

	/* I think this makes sense */
	/*
	start = X_AXIS + ((m_origin.x - X_AXIS) % (10 * m_cs.get_scale())) + step;

	for (int x = start; x < window->x; x += step)
	{
		bool flag = (x - m_origin.x) % 100 == 0 && (x - m_origin.x) != 0;

		if (flag)
		{

			std::string tag = std::to_string((x - m_origin.x) / m_cs.get_scale());
			SDL_Rect dim = m_helper->util_text_dim(&tag);

			m_helper->util_text(&tag, x + dim.h / 2, Y_AXIS - dim.w - 6, m_helper->palette()->white(), 90);
			m_helper->util_draw_line(x, Y_AXIS - 4, x, Y_AXIS + 4, m_helper->palette()->white());
			m_helper->util_draw_line(x, Y_AXIS + 4, x, window->y, m_helper->palette()->gray());
		}
		else
		{
			m_helper->util_draw_line(x, Y_AXIS - 2, x, Y_AXIS + 2, m_helper->palette()->white());
		}
	}

	start = Y_AXIS + ((m_origin.y - Y_AXIS) % (10 * m_scale_f)) + step;

	for (int y = start; y < window->y; y += step)
	{
		bool flag = (y - m_origin.y) % 100 == 0 && (y - m_origin.x) != 0;

		if (flag)
		{
			std::string tag = std::to_string((y - m_origin.y) / m_scale_f);
			SDL_Rect dim = m_helper->util_text_dim(&tag);

			m_helper->util_text(&tag, X_AXIS - dim.w - 5, y - dim.h / 2, m_helper->palette()->white());
			m_helper->util_draw_line(X_AXIS - 4, y, X_AXIS + 4, y, m_helper->palette()->white());
			m_helper->util_draw_line(X_AXIS + 4, y, window->x, y, m_helper->palette()->gray());
		}
		else
		{
			m_helper->util_draw_line(X_AXIS - 2, y, X_AXIS + 2, y, m_helper->palette()->white());
		}
	}

	m_helper->util_draw_line(0, Y_AXIS, window->x, Y_AXIS, m_helper->palette()->white());
	m_helper->util_draw_line(X_AXIS, 0, X_AXIS, window->y, m_helper->palette()->white());

	std::string t = "X in pixels";
	SDL_Rect dim = m_helper->util_text_dim(&t);

	m_helper->util_text(&t, X_AXIS - dim.w - 5, Y_AXIS - dim.h, m_helper->palette()->white());
	t = "Y in pixels";
	dim = m_helper->util_text_dim(&t);
	m_helper->util_text(&t, X_AXIS - dim.h - 5, Y_AXIS + 5, m_helper->palette()->white(), 90);

	t = "Scale: " + std::to_string(m_scale_f);
	dim = m_helper->util_text_dim(&t);
	m_helper->util_text(&t, window->x - dim.w - 5, 5, m_helper->palette()->white());

	SDL_Point mouse;
	SDL_GetMouseState(&mouse.x, &mouse.y);
	mouse.x = SDL_max(0, mouse.x - X_AXIS);
	mouse.y = SDL_max(0, mouse.y - Y_AXIS);
	t = "x: " + std::to_string(mouse.x) + " y:" + std::to_string(mouse.y);
	m_helper->util_text(&t, 5, 5, m_helper->palette()->white()); */
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
					- m_cs.get_origin()->x, e->button.y - (m_selected->get_y() * m_cs.get_scale())
					- m_cs.get_origin()->y };

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
			x = SDL_max((e->button.x - m_drag_element_offset.x -
				m_cs.get_origin()->x) / m_cs.get_scale(), 0);
			y = SDL_max((e->button.y - m_drag_element_offset.y -
				m_cs.get_origin()->x) / m_cs.get_scale(), 0);

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
}
