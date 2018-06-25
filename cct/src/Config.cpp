#include "Config.hpp"
#include "dialog/DialogElementSettings.hpp"

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
	/* Draw coordinate system */
	m_cs.draw_background();
	m_cs.draw_foreground();

	/* Draw elements */
	m_cs.begin_draw();
	{
		for (auto& const element : m_elements)
		{
			if (element.get() == m_selected)
				element->draw(m_atlas, &m_cs, true);
			else
				element->draw(m_atlas, &m_cs, false);
		}
	

		if (!SDL_RectEmpty(&m_total_selection))
		{
			SDL_Rect temp = m_total_selection;

			temp.x = temp.x * m_cs.get_scale() + m_cs.get_origin_x();
			temp.y = temp.y * m_cs.get_scale() + m_cs.get_origin_y();
			temp.w *= m_cs.get_scale();
			temp.h *= m_cs.get_scale();

			m_helper->util_draw_rect(&temp, m_helper->palette()->red());
		}

		if (!SDL_RectEmpty(&m_temp_selection))
		{
			m_helper->util_draw_rect(&m_temp_selection, m_helper->palette()->white());
		}

		if (m_element_to_delete >= 0 && m_element_to_delete < m_elements.size())
		{
			m_elements.erase(m_elements.begin() + m_element_to_delete);
			m_element_to_delete = -1;
		}
	}
	m_cs.end_draw();

}

void Config::handle_events(SDL_Event * e)
{
	m_cs.handle_events(e);

	if (e->type == SDL_MOUSEBUTTONDOWN)
	{
		if (e->button.button == SDL_BUTTON_LEFT)
		{
			/* Handle selection of elements */
			bool is_single_selection = false;
			bool selection_empty = SDL_RectEmpty(&m_total_selection);

			if (selection_empty)
			{
				int i = 0;
				for (auto& const elem : m_elements)
				{
					if (m_helper->util_is_in_rect(&elem.get()->get_abs_dim(&m_cs), e->button.x, e->button.y))
					{
						m_selected = elem.get();
						m_selected_id = i;
						
						m_dragging_element = true;
						m_drag_offset = { (e->button.x - (m_selected->get_x() * m_cs.get_scale())
							+ m_cs.get_origin()->x), (e->button.y - (m_selected->get_y() * m_cs.get_scale())
							+ m_cs.get_origin()->y) };

						m_settings->select_element(m_selected);
						is_single_selection = true;
						break;
					}
					i++;
				}

				if (!is_single_selection)
				/* No element was directly select -> start groups selection*/
				{
					m_selecting = true;
					reset_selected_element();
					m_selection_start = { e->button.x, e->button.y };
					m_selected_elements.clear();
					m_total_selection = {};
				}
			}
			else if (m_helper->util_is_in_rect(&m_total_selection, e->button.x, e->button.y))
			{
				m_dragging_elements = true;
				m_drag_offset = { e->button.x - m_total_selection.x,
					e->button.y - m_total_selection.y };
			}
			else
			{
				m_total_selection = {};
			}
		}
	}
	else if (e->type == SDL_MOUSEBUTTONUP)
	{
		m_dragging_element = false;
		m_dragging_elements = false;
		m_selecting = false;
		m_selected_elements.clear();
		m_temp_selection = {};
	}
	else if (e->type == SDL_MOUSEMOTION)
	{
		if (m_dragging_element && m_selected)
		{
			int x, y;
			x = SDL_max((e->button.x - m_drag_offset.x +
				m_cs.get_origin()->x) / m_cs.get_scale(), 0);
			y = SDL_max((e->button.y - m_drag_offset.y +
				m_cs.get_origin()->y) / m_cs.get_scale(), 0);

			m_selected->set_pos(x, y);
			m_settings->set_xy(x, y);
		}
		else if (m_selecting)
		{
			m_total_selection = {};
			SDL_Rect elem_dim;
			m_temp_selection.x = UTIL_MIN(e->button.x, m_selection_start.x);
			m_temp_selection.y = UTIL_MIN(e->button.y, m_selection_start.y);
			m_temp_selection.w = SDL_abs(e->button.x - m_selection_start.x);
			m_temp_selection.h = SDL_abs(e->button.y - m_selection_start.y);

			m_cs.translate(m_temp_selection.x, m_temp_selection.y);

			m_selected_elements.clear();
			int index = 0;
			for (auto& const elem : m_elements)
			{
				elem_dim = elem->get_dim();
				if (is_rect_in_rect(&elem_dim, &m_temp_selection))
				{
					m_selected_elements.emplace_back(index);
					SDL_UnionRect(&m_total_selection, &elem_dim, &m_total_selection);
				}
				index++;
			}
		}
		else if (m_dragging_elements)
		{
			m_total_selection.x = e->button.x - m_drag_offset.x;
			m_total_selection.y = e->button.y - m_drag_offset.y;
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
				m_settings->set_xy(x, y);
			}
		}

		for (auto& const element : m_elements)
		{
			if (e->key.keysym.sym == m_helper->vc_to_sdl_key(element->get_vc()))
			{
				element->set_pressed(true);
			}
		}
	}
	else if (e->type == SDL_KEYUP)
	{
		for (auto& const element : m_elements)
		{
			if (e->key.keysym.sym == m_helper->vc_to_sdl_key(element->get_vc()))
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

void Config::reset_selected_element(void)
{
	m_selected = nullptr;
	m_settings->set_id("");
	m_settings->set_uv(0, 0);
	m_settings->set_xy(0, 0);
	m_settings->set_wh(0, 0);
}


inline bool Config::is_rect_in_rect(const SDL_Rect * a, const SDL_Rect * b)
{
	return a->x >= b->x && a->x + a->w <= b->x + b->w
		&& a->y >= b->y && a->y + a->h <= b->y + b->h;
}
