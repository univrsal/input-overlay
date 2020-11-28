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

#include "element_analog_stick.hpp"
#include <keycodes.h>

#include "src/sources/input_source.hpp"

void element_analog_stick::load(const QJsonObject &obj)
{
    element_texture::load(obj);
    m_side = static_cast<element_side>(obj[CFG_SIDE].toInt());
    m_radius = static_cast<uint8_t>(obj[CFG_STICK_RADIUS].toInt());
    m_keycode = VC_STICK_DATA;
    m_pressed = m_mapping;
    m_pressed.y = m_mapping.y + m_mapping.cy + CFG_INNER_BORDER;
}

void element_analog_stick::draw(gs_effect_t *effect, gs_image_file_t *image, sources::overlay_settings *settings)
{
    auto pos = m_pos;
    gs_rect *temp = nullptr;

    if (m_side == element_side::LEFT)
        temp = settings->data.gamepad_buttons[gamepad::button::L_THUMB] ? &m_pressed : &m_mapping;
    else
        temp = settings->data.gamepad_buttons[gamepad::button::R_THUMB] ? &m_pressed : &m_mapping;
    //calc_position(&pos, stick, settings);
    element_texture::draw(effect, image, temp, &pos);
}

//void element_analog_stick::calc_position(vec2 *v, element_data_analog_stick *d,
//										 sources::overlay_settings *settings) const
//{
//	UNUSED_PARAMETER(settings);
//	/* TODO: deadzones combined with bindings? */
//	switch (m_side) {
//	case ES_LEFT:
//		//#if _WIN32
//		//		if (!DEAD_ZONE(d->get_left_stick()->x, settings->left_dz))
//		//#endif
//		//			v->x += d->get_left_stick()->x * m_radius;
//		//#if _WIN32
//		//		if (!DEAD_ZONE(d->get_left_stick()->y, settings->left_dz))
//		//#endif
//		//			v->y += d->get_left_stick()->y * m_radius;
//		//		break;
//		//	case ES_RIGHT:
//		//#if _WIN32
//		//		if (!DEAD_ZONE(d->get_right_stick()->x, settings->right_dz))
//		//#endif
//		//			v->x += d->get_right_stick()->x * m_radius;
//		//#if _WIN32
//		//		if (!DEAD_ZONE(d->get_right_stick()->y, settings->right_dz))
//		//#endif
//		v->y += d->get_right_stick()->y * m_radius;
//		break;
//	default:;
//	}
//}
