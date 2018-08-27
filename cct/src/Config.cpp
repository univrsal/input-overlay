/**
 * Created by universal on 27.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "Config.hpp"
#include "dialog/DialogElementSettings.hpp"
#include "util/Notifier.hpp"
#include <sstream>

#define X_AXIS 100
#define Y_AXIS 100

Config::Config(const char * texture, const char * config, SDL_Point def_dim, SDL_Point space, SDL_Helper * h,
	DialogElementSettings * s)
{
	m_texture_path = texture;
	m_config_path = config;

	m_settings = s;
	m_atlas = new Texture(texture, h->renderer());
	m_helper = h;
	m_default_dim = def_dim;
	m_offset = space;

	SDL_Point * w = h->util_window_size();
	m_cs = CoordinateSystem(SDL_Point { X_AXIS, Y_AXIS }, SDL_Rect { 0, 0, w->x, w->y }, h);

	if (def_dim.x > 0 && def_dim.y > 0)
		m_cs.set_grid_space(m_default_dim);
	if (space.x != 0 && space.y != 0)
		m_cs.set_ruler_offset(space);

	SDL_Point pos = { 0, 0 };
	SDL_Rect map = { 1, 1, 157, 128 };
}

Config::~Config()
{
	if (m_atlas)
		delete m_atlas;
	m_atlas = nullptr;
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

		int elements_drawn = 0;
		int layer = 0;

		while (elements_drawn < m_elements.size())
		{
			for (auto const &element : m_elements)
			{
				if (element->get_z_level() == layer)
				{
					element->draw(m_atlas, &m_cs, element.get() == m_selected, m_helper->is_ctrl_down());
					elements_drawn++;
				}
			}
			layer++;
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
			SDL_Rect temp = m_temp_selection;
			temp.x = temp.x * m_cs.get_scale() + m_cs.get_origin_x();
			temp.y = temp.y * m_cs.get_scale() + m_cs.get_origin_y();
			temp.w *= m_cs.get_scale();
			temp.h *= m_cs.get_scale();
			m_helper->util_draw_rect(&temp, m_helper->palette()->white());
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

			m_cs.translate(e->button.x, e->button.y);

			if (selection_empty)
			{
				int i = 0;
				int highest_layer_id = 0;
				int highest_layer = 0;

				for (auto const &elem : m_elements)
				{
					if (m_helper->util_is_in_rect(elem->get_abs_dim(&m_cs), e->button.x, e->button.y))
					{
						if (elem->get_z_level() >= highest_layer)
						{
							highest_layer = elem->get_z_level();
							highest_layer_id = i;
						}

						m_dragging_element = true;
						is_single_selection = true;
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
				else
				{
					m_selected = m_elements[highest_layer_id].get();
					m_drag_offset = { (e->button.x - (m_selected->get_x() * m_cs.get_scale())
						+ m_cs.get_origin()->x), (e->button.y - (m_selected->get_y() * m_cs.get_scale())
							+ m_cs.get_origin()->y) };
					m_settings->select_element(m_selected);
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
		m_temp_selection = {};
		m_selecting = false;
	}
	else if (e->type == SDL_MOUSEMOTION)
	{
		if (m_dragging_element && m_selected)
			/* Dragging single element */
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
			/* Selecting multiple elements */
		{
			m_total_selection = {};
			SDL_Rect elem_dim;
			SDL_Rect elem_abs_dim;

			m_temp_selection.x = UTIL_MIN(e->button.x, m_selection_start.x);
			m_temp_selection.y = UTIL_MIN(e->button.y, m_selection_start.y);

			m_cs.translate(m_temp_selection.x, m_temp_selection.y);

			m_temp_selection.x = ceil(UTIL_MAX((m_temp_selection.x - m_cs.get_origin_x()) /
				((float) m_cs.get_scale()), 0));
			m_temp_selection.y = ceil(UTIL_MAX((m_temp_selection.y - m_cs.get_origin_y()) /
				((float) m_cs.get_scale()), 0));

			m_temp_selection.w = ceil(SDL_abs(m_selection_start.x - e->button.x) / ((float) m_cs.get_scale()));
			m_temp_selection.h = ceil(SDL_abs(m_selection_start.y - e->button.y) / ((float) m_cs.get_scale()));

			m_selected_elements.clear();

			int index = 0;
			for (auto const &elem : m_elements)
			{
				if (is_rect_in_rect(elem->get_dim(), &m_temp_selection))
				{
					m_cs.translate(elem_abs_dim.x, elem_abs_dim.y);
					m_selected_elements.emplace_back(index);
					SDL_UnionRect(&m_total_selection, &elem_dim, &m_total_selection);
				}
				index++;
			}
		}
		else if (m_dragging_elements)
			/* Dragging multiple elements */
		{
			move_elements(e->button.x - m_drag_offset.x,
				e->button.y - m_drag_offset.y);
		}
	}
	else if (e->type == SDL_KEYDOWN)
	{
		if (m_selected)
			/* Move selected element with arrow keys */
		{
			int x = m_selected->get_x();
			int y = m_selected->get_y();

			if (util_move_element(&x, &y, e->key.keysym.sym))
			{
				m_selected->set_pos(x, y);
				m_settings->set_xy(x, y);
			}
		}
		else if (!m_selected_elements.empty())
		{
			int x = m_total_selection.x;
			int y = m_total_selection.y;

			if (util_move_element(&x, &y, e->key.keysym.sym))
				move_elements(x, y);
		}
	}
	else if (e->type == SDL_WINDOWEVENT)
	{
		if (e->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		{
			SDL_Point * w = m_helper->util_window_size();
			m_cs.set_dimensions(SDL_Rect { 0, 0, w->x, w->y });
		}
	}
}

void Config::write_config(Notifier * n)
{
	if (m_elements.empty())
	{
		n->add_msg(MESSAGE_INFO, m_helper->loc(LANG_MSG_NOTHING_TO_SAVE));
		return;
	}
	
	uint32_t start = SDL_GetTicks();
	ccl_config cfg = ccl_config(m_config_path, "CCT generated config");

	cfg.free_nodes(); /* Don't need existing values */

	cfg.add_string(CFG_FIRST_ID, "Starting point for loading elements", *m_elements[0]->get_id(), true);
	cfg.add_int(CFG_DEFAULT_WIDTH, "", m_default_dim.x);
	cfg.add_int(CFG_DEFAULT_HEIGHT, "Default element dimension", m_default_dim.y);
	cfg.add_int(CFG_H_SPACE, "", m_offset.x);
	cfg.add_int(CFG_V_SPACE, "Element offset for visual help", m_offset.y);

	int height = 0, width = 0; /* The most bottom right element determines the width/height */

	int index = 0;
	for (auto const &e : m_elements)
	{
		e->write_to_file(&cfg, &m_default_dim);

		width = UTIL_MAX(width, e->get_x() + e->get_w());
		height = UTIL_MAX(height, e->get_y() + e->get_h());

		if (index + 1 < m_elements.size())
		{
			cfg.add_string((*e->get_id()) + CFG_NEXT_ID, "Next element in list",
				*m_elements[index + 1]->get_id());
		}
		index++;
	}
	cfg.add_int(CFG_TOTAL_WIDTH, "", width);
	cfg.add_int(CFG_TOTAL_HEIGHT, "Full overlay dimensions", height);

	cfg.write();

	uint32_t end = SDL_GetTicks();

	if (cfg.has_fatal_errors())
	{
		n->add_msg(MESSAGE_ERROR, m_helper->loc(LANG_MSG_SAVE_ERROR));
		n->add_msg(MESSAGE_ERROR, cfg.get_error_message());
	}
	else
	{
		std::string result = m_helper->format(m_helper->loc(LANG_MSG_SAVE_SUCCESS).c_str(), m_elements.size(), (end - start));
		n->add_msg(MESSAGE_INFO, result);
	}
	cfg.free_nodes();
}

void Config::read_config(Notifier * n)
{
	uint32_t start = SDL_GetTicks();
	ccl_config cfg = ccl_config(m_config_path, "");

	if (cfg.is_empty())
	{
		n->add_msg(MESSAGE_INFO, m_helper->loc(LANG_MSG_CONFIG_EMPTY));
		return;
	}

	if (!cfg.node_exists(CFG_FIRST_ID, true))
	{
		n->add_msg(MESSAGE_INFO, m_helper->loc(LANG_MSG_CONFIG_CORRUPT));
		return;
	}

	std::string element_id = cfg.get_string(CFG_FIRST_ID);

	if (cfg.get_int(CFG_DEFAULT_WIDTH) != 0)
		m_default_dim.x = cfg.get_int(CFG_DEFAULT_WIDTH);
	if (cfg.get_int(CFG_DEFAULT_HEIGHT) != 0)
		m_default_dim.y = cfg.get_int(CFG_DEFAULT_HEIGHT);

	int type = 0;

	while (!element_id.empty())
	{
		type = cfg.get_int(element_id + CFG_TYPE);
		if (!Element::valid_type(type))
		{
			std::string result = m_helper->format(m_helper->loc(LANG_MSG_VALUE_TYPE_INVALID).c_str(), element_id.c_str(), type);
			n->add_msg(MESSAGE_ERROR, result);

			element_id = cfg.get_string(element_id + CFG_NEXT_ID);
			continue;
		}
		m_elements.emplace_back(Element::read_from_file(&cfg, element_id, (ElementType) type, &m_default_dim));

		element_id = cfg.get_string(element_id + CFG_NEXT_ID);
	}

	uint32_t end = SDL_GetTicks();

	if (cfg.has_fatal_errors())
	{
		n->add_msg(MESSAGE_ERROR, m_helper->loc(LANG_MSG_LOAD_ERROR));
		n->add_msg(MESSAGE_ERROR, cfg.get_error_message());
	}
	else
	{
		std::string result = m_helper->format(m_helper->loc(LANG_MSG_LOAD_SUCCESS).c_str(), m_elements.size(), (end - start));
		n->add_msg(MESSAGE_INFO, result);
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
	m_settings->set_vc(0);
}

void Config::move_elements(int new_x, int new_y)
{
	if (!m_selected_elements.empty())
	{
		int delta_x = new_x - m_total_selection.x;
		int delta_y = new_y - m_total_selection.y;

		bool flag_x = new_x >= 0, flag_y = new_y >= 0;

		Element * e = nullptr;

		if (flag_x)
			m_total_selection.x = new_x;
		if (flag_y)
			m_total_selection.y = new_y;

		if (flag_x || flag_y)
			for (auto const &index : m_selected_elements)
			{
				if (index < m_elements.size())
				{
					e = m_elements[index].get();
					if (e)
					{
						e->set_pos(
							e->get_x() + (flag_x ? delta_x : 0),
							e->get_y() + (flag_y ? delta_y : 0));
					}
				}
			}

	}
}

inline bool Config::is_rect_in_rect(const SDL_Rect * a, const SDL_Rect * b)
{
	return a->x >= b->x && a->x + a->w <= b->x + b->w
		&& a->y >= b->y && a->y + a->h <= b->y + b->h;
}
