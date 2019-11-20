/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2019 univrsal <universailp@web.de>.
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

#include "../../sources/input_source.hpp"
#include "../../../ccl/ccl.hpp"
#include "element_trigger.hpp"
#include "../util.hpp"
#include "util/layout_constants.hpp"

element_trigger::element_trigger() : element_texture(ET_TRIGGER) {}

void element_trigger::load(ccl_config *cfg, const std::string &id)
{
	element_texture::load(cfg, id);
	m_button_mode = cfg->get_bool(id + CFG_TRIGGER_MODE);
	m_side = static_cast<element_side>(cfg->get_int(id + CFG_SIDE));
	m_keycode = VC_TRIGGER_DATA;
	m_pressed = m_mapping;
	m_pressed.y = m_mapping.y + m_mapping.cy + CFG_INNER_BORDER;
	if (!m_button_mode) {
		m_direction = static_cast<direction>(cfg->get_int(id + CFG_DIRECTION));
	}
}

void element_trigger::draw(gs_effect_t *effect, gs_image_file_t *image, element_data *data,
                           sources::overlay_settings *settings)
{
    UNUSED_PARAMETER(settings);

	if (data) {
		const auto trigger = dynamic_cast<element_data_trigger *>(data);
		auto progress = 0.f;
		if (trigger) {
			switch (m_side) {
			case ES_LEFT:
				progress = trigger->get_left();
				break;
			case ES_RIGHT:
				progress = trigger->get_right();
				break;
			default:;
			}

			if (m_button_mode) {
				if (progress >= 0.1) {
					element_texture::draw(effect, image, &m_pressed);
				} else {
					element_texture::draw(effect, image, &m_mapping);
				}
			} else {
				auto crop = m_pressed;
				auto new_pos = m_pos;
				calculate_mapping(&crop, &new_pos, progress);
				element_texture::draw(effect, image, &m_mapping); /* Draw unpressed first */
				element_texture::draw(effect, image, &crop, &new_pos);
			}
		}
	} else {
		element_texture::draw(effect, image, nullptr);
	}
}

data_source element_trigger::get_source()
{
	return DS_GAMEPAD;
}

void element_trigger::calculate_mapping(gs_rect *pressed, vec2 *pos, const float progress) const
{
	switch (m_direction) {
	case DIR_UP:
		pressed->cy = m_mapping.cy * progress;
		pressed->y = m_pressed.y + (m_mapping.cy - pressed->cy);
		pos->y += m_mapping.cy - pressed->cy;
		break;
	case DIR_DOWN:
		pressed->cy = m_mapping.cy * progress;
		break;
	case DIR_LEFT:
		pressed->cx = m_mapping.cx * progress;
		pressed->x = m_mapping.x + (m_mapping.cx - pressed->cx);
		pos->x += (m_mapping.cx - pressed->cx);
		break;
	case DIR_RIGHT:
		pressed->cx = m_mapping.cx * progress;
		break;
	}
}

element_data_trigger::element_data_trigger(const trigger_data side, const float val) : element_data(ET_TRIGGER)
{
	if (side == TD_LEFT)
		m_left_trigger = val;
	else
		m_right_trigger = val;
	m_data_type = side;
}

element_data_trigger::element_data_trigger(const float left, const float right) : element_data(ET_TRIGGER)
{
	m_left_trigger = left;
	m_right_trigger = right;
	m_data_type = TD_BOTH;
}

float element_data_trigger::get_left() const
{
	return m_left_trigger;
}

float element_data_trigger::get_right() const
{
	return m_right_trigger;
}

bool element_data_trigger::is_persistent()
{
	return true;
}

bool element_data_trigger::merge(element_data *other)
{
	if (other && other->get_type() == m_type) {
		const auto trigger = dynamic_cast<element_data_trigger *>(other);

		if (trigger) {
			switch (trigger->m_data_type) {
			case TD_BOTH:
				m_left_trigger = trigger->m_left_trigger;
				m_right_trigger = trigger->m_right_trigger;
				break;
			case TD_LEFT:
				m_left_trigger = trigger->m_left_trigger;
				if (m_data_type == TD_RIGHT) /* Left merged with right = now contains both sides */
					m_data_type = TD_BOTH;
				break;
			case TD_RIGHT:
				m_right_trigger = trigger->m_right_trigger;
				if (m_data_type == TD_LEFT) /* Left merged with right = now contains both sides */
					m_data_type = TD_BOTH;
				break;
			default:;
			}
		}
	}
	return false;
}

element_data_trigger *element_data_trigger::from_buffer(netlib_byte_buf *buffer)
{
	uint8_t left, right;
	if (!netlib_read_uint8(buffer, &left) || !netlib_read_uint8(buffer, &right)) {
#ifdef _DEBUG
		blog(LOG_INFO, "Failed to read trigger data: %s", netlib_get_error());
#endif
		return nullptr;
	}
	return new element_data_trigger(left / TRIGGER_MAX_VAL, right / TRIGGER_MAX_VAL);
}
