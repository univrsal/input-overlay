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

#include "config.hpp"
#include "dialog/dialog_element_settings.hpp"
#include "element/element_analog_stick.hpp"
#include "util/constants.hpp"
#include "util/notifier.hpp"
#include "util/palette.hpp"
#include "util/sdl_helper.hpp"
#include "util/texture.hpp"
#include <iomanip>
#include <fstream>

config::config(const char *texture_path, const char *config, const SDL_Point def_dim, const SDL_Point space,
			   sdl_helper *h, dialog_element_settings *s)
{
	m_texture_path = texture_path;
	m_config_path = config;

	m_settings = s;
	m_atlas = new texture(texture_path, h->renderer());
	m_helper = h;
	m_default_dim = def_dim;
	m_offset = space;

	const auto w = h->util_window_size();
	m_cs = coordinate_system(SDL_Point{X_AXIS, Y_AXIS}, SDL_Rect{0, 0, w->x, w->y}, h);

	if (def_dim.x > 0 && def_dim.y > 0)
		m_cs.set_grid_space(m_default_dim);
	if (space.x != 0 && space.y != 0)
		m_cs.set_ruler_offset(space);
}

config::~config()
{
	delete m_atlas;
	m_atlas = nullptr;
	m_helper = nullptr;
	m_settings = nullptr;
}

void config::draw_elements()
{
	/* Draw coordinate system */
	m_cs.draw_background();
	m_cs.draw_foreground();

	/* Draw elements */
	m_cs.begin_draw();
	{
		auto elements_drawn = 0ull;
		auto layer = 0;

		while (elements_drawn < m_elements.size()) {
			for (auto const &element : m_elements) {
				if (element->get_z_level() == layer) {
					element->draw(m_atlas, &m_cs, element.get() == m_selected, m_helper->is_ctrl_down());
					elements_drawn++;
				}
			}
			layer++;
		}

		if (!SDL_RectEmpty(&m_total_selection)) {
			auto temp = m_total_selection;

			temp.x = temp.x * m_cs.get_scale() + m_cs.get_origin_x();
			temp.y = temp.y * m_cs.get_scale() + m_cs.get_origin_y();
			temp.w *= m_cs.get_scale();
			temp.h *= m_cs.get_scale();

			m_helper->util_draw_rect(&temp, m_helper->get_palette()->red());
		}

		if (!SDL_RectEmpty(&m_temp_selection)) {
			auto temp = m_temp_selection;
			temp.x = temp.x * m_cs.get_scale() + m_cs.get_origin_x();
			temp.y = temp.y * m_cs.get_scale() + m_cs.get_origin_y();
			temp.w *= m_cs.get_scale();
			temp.h *= m_cs.get_scale();
			m_helper->util_draw_rect(&temp, m_helper->get_palette()->white());
		}

		if (m_element_to_delete >= 0 && m_element_to_delete < m_elements.size()) {
			m_elements.erase(m_elements.begin() + m_element_to_delete);
			if (m_element_to_delete == m_selected_id) {
				m_selected = nullptr;
				m_selected_id = -1;
			}
			m_element_to_delete = -1;
		}
	}
	m_cs.end_draw();
}

void config::handle_events(SDL_Event *e)
{
	m_cs.handle_events(e);
	auto m_x = e->button.x, m_y = e->button.y;

	if (e->type == SDL_MOUSEBUTTONDOWN) {
		if (e->button.button == SDL_BUTTON_LEFT) {
			/* Handle selection of elements */
			m_cs.translate(m_x, m_y);

			if (SDL_RectEmpty(&m_total_selection))
			/* No multiple items already selected */ {
				auto i = 0;
				auto highest_layer = 0;
				m_selected_id = -1;

				for (auto const &elem : m_elements) {
					if (sdl_helper::util_is_in_rect(elem->get_abs_dim(&m_cs), m_x, m_y)) {
						if (elem->get_z_level() >= highest_layer) {
							highest_layer = elem->get_z_level();
							m_selected = m_elements[i].get();
							m_selected_id = i;
						}
						m_in_single_selection = true;
					}
					i++;
				}

				if (m_in_single_selection)
				/* Start single element selection */ {
					m_drag_offset = {(e->button.x - (m_selected->get_x() * m_cs.get_scale()) + m_cs.get_origin()->x),
									 (e->button.y - (m_selected->get_y() * m_cs.get_scale()) + m_cs.get_origin()->y)};
					m_settings->select_element(m_selected);
				} else /* Start multi element selection */ {
					m_in_multi_selection = true;
					reset_selection();
					m_selection_start = {e->button.x, e->button.y};
				}
			} else if (sdl_helper::util_is_in_rect(&m_total_selection, e->button.x, e->button.y)) {
				m_dragging_elements = true;
				m_drag_offset = {e->button.x - m_total_selection.x, e->button.y - m_total_selection.y};
			} else {
				m_total_selection = {};
			}
		}
	} else if (e->type == SDL_MOUSEBUTTONUP) {
		m_in_single_selection = false;
		m_dragging_elements = false;
		m_temp_selection = {};
		m_in_multi_selection = false;
	} else if (e->type == SDL_MOUSEMOTION) {
		if (m_in_single_selection && m_selected)
		/* Dragging single element */ {
			move_element(e->button.x, e->button.y);
		} else if (m_in_multi_selection)
		/* Selecting multiple elements */ {
			SDL_Rect elem_dim;
			SDL_Rect elem_abs_dim;

			m_temp_selection.x = UTIL_MIN(e->button.x, m_selection_start.x);
			m_temp_selection.y = UTIL_MIN(e->button.y, m_selection_start.y);

			m_cs.translate(m_temp_selection.x, m_temp_selection.y);

			m_temp_selection.x =
				int(ceil(UTIL_MAX((m_temp_selection.x - m_cs.get_origin_x()) / ((float)m_cs.get_scale()), 0)));
			m_temp_selection.y =
				int(ceil(UTIL_MAX((m_temp_selection.y - m_cs.get_origin_y()) / ((float)m_cs.get_scale()), 0)));

			m_temp_selection.w =
				int(ceil(SDL_abs(m_selection_start.x - e->button.x) / static_cast<float>(m_cs.get_scale())));
			m_temp_selection.h =
				int(ceil(SDL_abs(m_selection_start.y - e->button.y) / static_cast<float>(m_cs.get_scale())));

			m_selected_elements.clear();

			uint16_t index = 0;
			for (auto const &elem : m_elements) {
				if (is_rect_in_rect(elem->get_mapping(), &m_temp_selection)) {
					m_cs.translate(elem_abs_dim.x, elem_abs_dim.y);
					m_selected_elements.emplace_back(index);
					SDL_UnionRect(&m_total_selection, &elem_dim, &m_total_selection);
				}
				index++;
			}
		} else if (m_dragging_elements)
		/* Dragging multiple elements */ {
			move_elements(e->button.x - m_drag_offset.x, e->button.y - m_drag_offset.y);
		}
	} else if (e->type == SDL_KEYDOWN) {
		if (m_selected)
		/* Move selected element with arrow keys */ {
			auto x = m_selected->get_x();
			auto y = m_selected->get_y();

			if (util_move_element(&x, &y, e->key.keysym.sym)) {
				m_selected->set_pos(x, y);
				m_settings->set_xy(x, y);
			}
		} else if (!m_selected_elements.empty()) {
			auto x = m_total_selection.x, y = m_total_selection.y;

			if (util_move_element(&x, &y, e->key.keysym.sym))
				move_elements(x, y);
		}
	} else if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
		const auto w = m_helper->util_window_size();
		m_cs.set_dimensions(SDL_Rect{0, 0, w->x, w->y});
	}

	/* Let elements react to SDL events */
	for (auto &element : m_elements)
		element->handle_event(e, m_helper);
}

void config::write_config(notifier *n)
{
	if (m_elements.empty()) {
		n->add_msg(MESSAGE_INFO, m_helper->loc(LANG_MSG_NOTHING_TO_SAVE));
		return;
	}

	const auto start = SDL_GetTicks();
#if WIN32
	std::ofstream out(sdl_helper::util_utf8_to_wstring(m_config_path));
#else

	std::ofstream out(m_config_path);
#endif
	/* The most bottom right element determines the width/height */
	auto height = 0, width = 0;
	std::vector<json> elements;
	uint8_t flags = 0; /* Determines which properties to show in OBS */

	for (auto &e : m_elements) {
		json::object j;
		e->write_to_json(j, &m_default_dim, flags);
		if (j.empty()) {
			n->add_msg(MESSAGE_ERROR, m_helper->format_loc(LANG_MSG_ELEMENT_EMPTY, e->get_id()->c_str()));
			continue;
		}
		elements.emplace_back(j);
		width = UTIL_MAX(width, e->get_x() + e->get_w());
		height = UTIL_MAX(height, e->get_y() + e->get_h());
	}

	json cfg = json::object{{CFG_DEFAULT_WIDTH, m_default_dim.x},
							{CFG_DEFAULT_HEIGHT, m_default_dim.y},
							{CFG_H_SPACE, m_offset.x},
							{CFG_V_SPACE, m_offset.y},
							{CFG_TOTAL_WIDTH, width},
							{CFG_TOTAL_HEIGHT, height},
							{CFG_FLAGS, flags},
							{CFG_ELEMENTS, elements}};
	std::string json_str = cfg.dump();
	util::indent_json(json_str);
	out << json_str;

	const auto end = SDL_GetTicks();

	if (out.good()) {
		const auto result =
			sdl_helper::format(m_helper->loc(LANG_MSG_SAVE_SUCCESS).c_str(), m_elements.size(), (end - start));
		n->add_msg(MESSAGE_INFO, result);
	} else {
		n->add_msg(MESSAGE_ERROR, m_helper->loc(LANG_MSG_SAVE_ERROR));
		n->add_msg(MESSAGE_ERROR, m_helper->loc(LANG_MSG_ACCESS_ERROR));
	}
}

void config::read_config(notifier *n)
{
	const auto start = SDL_GetTicks();
	json cfg;
	std::string err;
	if (!util::load_json(m_config_path, err, cfg)) {
		n->add_msg(MESSAGE_ERROR, m_helper->loc(LANG_MSG_LOAD_ERROR));
		n->add_msg(MESSAGE_ERROR, err);
		return;
	}

	auto elements = cfg[CFG_ELEMENTS];
	if (!elements.is_array()) {
		n->add_msg(MESSAGE_INFO, m_helper->loc(LANG_MSG_CONFIG_NO_ELEMENTS));
		return;
	}

	auto def_w = cfg[CFG_DEFAULT_WIDTH];
	auto def_h = cfg[CFG_DEFAULT_HEIGHT];

	if (def_w.is_number())
		m_default_dim.x = def_w.int_value();
	if (def_h.is_number())
		m_default_dim.y = def_h.int_value();

	for (const auto &element : elements.array_items()) {
		auto type = element[CFG_TYPE];
		if (type.is_number() && element::valid_type(type.int_value())) {
			auto loaded_element = element::read_from_json(element, &m_default_dim);
			if (loaded_element) {
				m_elements.emplace_back(loaded_element);
			} else {
				n->add_msg(MESSAGE_ERROR, m_helper->format_loc(LANG_MSG_ELEMENT_LOAD_ERROR, element[CFG_ID]));
			}
		} else {
			n->add_msg(MESSAGE_ERROR, m_helper->format_loc(LANG_MSG_VALUE_TYPE_INVALID, element[CFG_ID], type));
		}
	}

	const auto end = SDL_GetTicks();

	if (m_elements.empty()) {
		n->add_msg(MESSAGE_ERROR, m_helper->loc(LANG_MSG_LOAD_ERROR));
		n->add_msg(MESSAGE_ERROR, "TODO");
	} else {
		const auto result =
			sdl_helper::format(m_helper->loc(LANG_MSG_LOAD_SUCCESS).c_str(), m_elements.size(), (end - start));
		n->add_msg(MESSAGE_INFO, result);
	}
}

texture *config::get_texture() const
{
	return m_atlas;
}

SDL_Point config::get_default_dim() const
{
	return m_default_dim;
}

void config::reset_selection()
{
	m_selected = nullptr;
	m_settings->set_id("");
	m_settings->set_uv(0, 0);
	m_settings->set_xy(0, 0);
	m_settings->set_wh(0, 0);
	m_settings->set_vc(0);
	m_selected_elements.clear();
	m_total_selection = {};
}

void config::move_elements(const int new_x, const int new_y)
{
	if (!m_selected_elements.empty()) {
		const auto delta_x = new_x - m_total_selection.x;
		const auto delta_y = new_y - m_total_selection.y;

		const auto flag_x = new_x >= 0, flag_y = new_y >= 0;

		if (flag_x)
			m_total_selection.x = new_x;
		if (flag_y)
			m_total_selection.y = new_y;

		if (flag_x || flag_y)
			for (auto const &index : m_selected_elements) {
				if (index < m_elements.size()) {
					auto e = m_elements[index].get();
					if (e) {
						e->set_pos(e->get_x() + (flag_x ? delta_x : 0), e->get_y() + (flag_y ? delta_y : 0));
					}
				}
			}
	}
}

inline void config::move_element(const int mouse_x, const int mouse_y)
{
	const auto x = SDL_max((mouse_x - m_drag_offset.x + m_cs.get_origin()->x) / m_cs.get_scale(), 0);
	const auto y = SDL_max((mouse_y - m_drag_offset.y + m_cs.get_origin()->y) / m_cs.get_scale(), 0);

	m_selected->set_pos(x, y);
	m_settings->set_xy(x, y);
}

inline bool config::is_rect_in_rect(const SDL_Rect *a, const SDL_Rect *b)
{
	return a->x >= b->x && a->x + a->w <= b->x + b->w && a->y >= b->y && a->y + a->h <= b->y + b->h;
}
