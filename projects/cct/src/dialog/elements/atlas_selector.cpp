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

#include "atlas_selector.hpp"
#include "../../util/constants.hpp"

atlas_selector::atlas_selector(const int8_t id, const int x, const int y, const int w, const int h, texture *t,
							   dialog *parent)
{
	const SDL_Rect temp = {x, y, w, h};
	atlas_selector::init(parent, temp, id);
	m_atlas = t;
}

atlas_selector::~atlas_selector()
{
	atlas_selector::close();
}

void atlas_selector::close()
{
	gui_element::close();

	delete m_cs;
	m_cs = nullptr;
}

void atlas_selector::init(dialog *parent, const SDL_Rect dim, const int8_t id)
{
	gui_element::init(parent, dim, id);
	m_cs = new coordinate_system(SDL_Point{X_AXIS, Y_AXIS}, m_dimensions, get_helper());
	m_cs->enable_border();
	m_cs->enable_crosshair();
}

void atlas_selector::draw_foreground()
{
	gui_element::draw_foreground();

	auto temp = m_atlas->get_dim();
	temp.w *= m_cs->get_scale();
	temp.h *= m_cs->get_scale();
	temp.x = m_cs->get_origin_x();
	temp.y = m_cs->get_origin_y();

	m_cs->draw_foreground();

	m_cs->begin_draw();
	{
		m_atlas->draw(get_helper()->renderer(), &temp, nullptr);
	}
	m_cs->end_draw();

	m_cs->draw_selection();
}

void atlas_selector::draw_background()
{
	m_cs->draw_background();
}

bool atlas_selector::handle_events(SDL_Event *event, bool was_handled)
{
	auto handled = false;

	if (m_cs->handle_events(event))
		handled = true;

	return handled;
}

uint8_t atlas_selector::get_cursor()
{
	return CURSOR_SPECIAL;
}

void atlas_selector::resize()
{
	m_cs->set_pos(m_dimensions.x, m_dimensions.y);
	m_cs->set_dimensions(m_dimensions);
}
