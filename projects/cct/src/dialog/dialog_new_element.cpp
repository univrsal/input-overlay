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

#include "dialog_new_element.hpp"
#include "../config.hpp"
#include "../element/element_analog_stick.hpp"
#include "../element/element_mouse_movement.hpp"
#include "../element/element_trigger.hpp"
#include "../util/constants.hpp"

void dialog_new_element::load_from_element(element *e)
{
	if (e) {
		if (m_u && m_v && m_w && m_h) {
			m_u->set_text(std::to_string(e->get_u()));
			m_v->set_text(std::to_string(e->get_v()));

			m_w->set_text(std::to_string(e->get_w()));
			m_h->set_text(std::to_string(e->get_h()));
		}
		m_z_level->set_text(std::to_string(e->get_z_level()));

		m_element_id->set_text(*e->get_id());
		m_initial_name = *e->get_id();

		if (m_keycode)
			m_keycode->set_hex_int(e->get_vc());

		m_selection.x = e->get_u();
		m_selection.y = e->get_v();
		m_selection.w = e->get_w();
		m_selection.h = e->get_h();

		m_modify_mode = true;

		/* Specific handling */
		element_analog_stick *stick = nullptr;
		element_mouse_movement *mouse = nullptr;
		element_trigger *trigger = nullptr;

		switch (e->get_type()) {
		case ET_ANALOG_STICK:
			stick = dynamic_cast<element_analog_stick *>(e);
			m_binary_choice->select_item(stick->get_stick());
			m_radius->set_text(std::to_string(stick->get_radius()));
			break;
		case ET_MOUSE_STATS:
			mouse = dynamic_cast<element_mouse_movement *>(e);
			m_binary_choice->select_item(mouse->get_mouse_type());
			m_radius->set_text(std::to_string(mouse->get_radius()));
			break;
		case ET_TRIGGER:
			trigger = dynamic_cast<element_trigger *>(e);
			m_binary_choice->select_item(trigger->get_side());
			m_direction->select_item(trigger->get_direction());
			m_trigger_mode->set_checked(trigger->get_mode());
			break;
		default:;
		}
	}
}

void dialog_new_element::init()
{
	set_flags(DIALOG_TOP_MOST | DIALOG_FLUID);

	dialog::init();
	m_element_y = 30;

	add_element_id();
	add_z_level();

	if (m_type == ET_ANALOG_STICK)
		add_mouse_or_analog_stick(LANG_LABEL_STICK_SIDE, LANG_LEFT, LANG_RIGHT);
	else if (m_type == ET_BUTTON)
		add_keycode_elements();
	else if (m_type == ET_WHEEL)
		add_info(LANG_LABEL_WHEEL_INFO);
	else if (m_type == ET_MOUSE_STATS)
		add_mouse_or_analog_stick(LANG_LABEL_MOUSE_TYPE, LANG_ITEM_MOUSE_TYPE_DOT, LANG_ITEM_MOUSE_TYPE_ARROW);
	else if (m_type == ET_TRIGGER)
		add_trigger();
	else if (m_type == ET_GAMEPAD_ID)
		add_info(LANG_LABEL_GAMEPAD_ID_INFO);
	else if (m_type == ET_DPAD_STICK)
		add_info(LANG_LABEL_DPAD_INFO);

	switch (m_type) {
	case ET_ANALOG_STICK:
	case ET_BUTTON:
	case ET_TEXTURE:
	case ET_WHEEL:
	case ET_MOUSE_STATS:
	case ET_TRIGGER:
	case ET_GAMEPAD_ID:
	case ET_DPAD_STICK:
		add(m_selector = new atlas_selector(get_left() + 270, get_top() + 30, m_dimensions.w - 278, m_dimensions.h - 38,
											m_tool->get_atlas(), this));
		m_selector->set_selection(&m_selection);
		add_selection_elements();
		break;
	case ET_INVALID:
	default:;
	}

	m_ok = add<button>(8, m_dimensions.h - 32, LANG_BUTTON_OK, this);
	m_cancel = add<button>(124, m_dimensions.h - 32, LANG_BUTTON_CANCEL, this);
	m_cancel->set_id(ACTION_CANCEL);
	m_ok->set_id(ACTION_OK);
}

void dialog_new_element::action_performed(const int8_t action_id)
{
	element_error error;
	const auto name_unmodified = m_initial_name == m_element_id->c_str();

	switch (action_id) {
	case ACTION_OK:
		error = m_tool->verify_element(this, m_modify_mode && name_unmodified);
		if (error == EE_VALID) {
			if (m_modify_mode)
				m_tool->action_performed(TOOL_ACTION_MOD_ELEMENT_APPLY);
			else
				m_tool->action_performed(TOOL_ACTION_NEW_ELEMENT_ADD);
		} else {
			handle_error(error);
		}
		break;
	case ACTION_CANCEL:
		m_tool->queue_dialog_close();
		break;
	case ACTION_TEXT_TYPED:
		if (m_selector && !m_selector->selection_changing() && m_u && m_v && m_w && m_h) {
			m_selection.x = SDL_strtol(m_u->c_str(), nullptr, 10);
			m_selection.y = SDL_strtol(m_v->c_str(), nullptr, 10);
			m_selection.w = SDL_strtol(m_w->c_str(), nullptr, 10);
			m_selection.h = SDL_strtol(m_h->c_str(), nullptr, 10);
		}
		break;
	default:
		if (m_read_keybind) {
			if (m_read_keybind->get_state())
				m_keycode->set_flags(TEXTBOX_HEX | TEXTBOX_KEYBIND);
			else
				m_keycode->set_flags(TEXTBOX_HEX);
		}
	}
}

void dialog_new_element::draw_background()
{
	dialog::draw_background();
}

void dialog_new_element::draw_foreground()
{
	dialog::draw_foreground();
}

bool dialog_new_element::handle_events(SDL_Event *event)
{
	auto was_handled = false;
	if (dialog::handle_events(event))
		was_handled = true;

	switch (m_type) {
	case ET_TEXTURE:
	case ET_BUTTON:
	case ET_MOUSE_STATS:
	case ET_ANALOG_STICK:
	case ET_WHEEL:
	case ET_GAMEPAD_ID:
	case ET_TRIGGER:
	case ET_DPAD_STICK:
		if (event->type == SDL_WINDOWEVENT) {
			if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
				const struct SDL_Rect t = {get_left() + 270, get_top() + 30, m_dimensions.w - 278, m_dimensions.h - 38};
				m_selector->set_dim(t);
				m_ok->set_pos(8, m_dimensions.h - 32);
				m_cancel->set_pos(124, m_dimensions.h - 32);
			}
		}

		if (!m_selector->selection_changing()) {
			m_h->set_text(std::to_string(m_selection.h));
			m_w->set_text(std::to_string(m_selection.w));
			m_u->set_text(std::to_string(m_selection.x));
			m_v->set_text(std::to_string(m_selection.y));
		}

		break;
	default:;
	}

	return was_handled;
}

element_type dialog_new_element::get_type() const
{
	return m_type;
}

SDL_Rect dialog_new_element::get_selection() const
{
	return m_selection;
}

uint16_t dialog_new_element::get_vc() const
{
	uint16_t t = 0x0;

	if (m_keycode) {
		t = static_cast<uint16_t>(strtoul(m_keycode->get_text()->c_str(), nullptr, 16));
	}

	return t;
}

uint8_t dialog_new_element::get_z_level() const
{
	return uint8_t(SDL_strtol(m_z_level->get_text()->c_str(), nullptr, 10));
}

element_side dialog_new_element::get_side() const
{
	if (m_binary_choice) {
		switch (m_binary_choice->get_selected()) {
		case ES_RIGHT:
			return ES_RIGHT;
		default:
			return ES_LEFT;
		}
	}
	return ES_LEFT;
}

direction dialog_new_element::get_direction() const
{
	if (m_direction) {
		return static_cast<direction>(UTIL_CLAMP(0, m_direction->get_selected(), DIR_MAX - 1));
	}
	return DIR_UP;
}

mouse_movement dialog_new_element::get_mouse_type() const
{
	if (m_binary_choice) {
		switch (m_binary_choice->get_selected()) {
		case MM_DOT:
			return MM_DOT;
		default:
			return MM_ARROW;
		}
	}
	return MM_DOT;
}

uint8_t dialog_new_element::get_radius() const
{
	if (m_radius)
		return uint8_t(SDL_strtol(m_radius->c_str(), nullptr, 10));
	return 0;
}

const std::string *dialog_new_element::get_id() const
{
	return m_element_id->get_text();
}

void dialog_new_element::set_default_dim(int w, int h)
{
	m_selection.w = w;
	m_selection.h = h;
	m_selection.x = 1;
	m_selection.y = 1;

	if (m_w && m_h && m_u && m_v) {
		m_w->set_text(std::to_string(w));
		m_h->set_text(std::to_string(h));
		m_u->set_text(std::to_string(1));
		m_v->set_text(std::to_string(1));
	}
}

void dialog_new_element::handle_error(element_error e) const
{
	switch (e) {
	case EE_ID_EMPTY:
	case EE_ID_NOT_UNIQUE:
		m_element_id->set_alert(true);
		break;
	case EE_MAPPING_EMPTY:
		m_u->set_alert(true);
		m_v->set_alert(true);
		m_w->set_alert(true);
		m_h->set_alert(true);
		break;
	case EE_KEYCODE_INVALID:
		m_keycode->set_alert(true);
		break;
	case EE_STICK_RADIUS:
		m_radius->set_alert(true);
		break;
	default:;
	}
}

void dialog_new_element::add_selection_elements()
{
	m_selection = {0, 0, 0, 0};

	if (m_element_y == 0)
		m_element_y = 30;

	add<label>(8, m_element_y, LANG_LABEL_TEXTURE_SELECTION, this);

	m_element_y += 25;
	add<label>(8, m_element_y, LANG_LABEL_WIDTH, this);
	add<label>(16 + panel_w / 2, m_element_y, LANG_LABEL_HEIGHT, this);
	m_w = add<textbox>(8 + panel_w / 4 + 4, m_element_y, panel_w / 4, 20, "0", this);
	m_h = add<textbox>(8 + panel_w / 4 * 3 + 4, m_element_y, panel_w / 4, 20, "0", this);

	m_element_y += 25;
	add<label>(8, m_element_y, LANG_LABEL_U, this);
	add<label>(16 + panel_w / 2, m_element_y, LANG_LABEL_V, this);
	m_u = add<textbox>(8 + panel_w / 4 + 4, m_element_y, panel_w / 4, 20, "0", this);
	m_v = add<textbox>(8 + panel_w / 4 * 3 + 4, m_element_y, panel_w / 4, 20, "0", this);

	m_w->set_flags(TEXTBOX_NUMERIC);
	m_w->set_cutoff(20);
	m_h->set_flags(TEXTBOX_NUMERIC);
	m_h->set_cutoff(20);
	m_u->set_flags(TEXTBOX_NUMERIC);
	m_u->set_cutoff(20);
	m_v->set_flags(TEXTBOX_NUMERIC);
	m_v->set_cutoff(20);

	m_element_y += 40;
}

void dialog_new_element::add_keycode_elements()
{
	if (m_element_y == 0)
		m_element_y = 30;
	add<label>(8, m_element_y, LANG_LABEL_KEY_CODE, this);
	m_element_y += 25;
	m_keycode = add<textbox>(8, m_element_y, panel_w, 20, "0", this);
	m_keycode->set_cutoff(10);
	m_keycode->set_flags(TEXTBOX_HEX | TEXTBOX_KEYBIND);
	m_element_y += 25;
	m_read_keybind = add<checkbox>(8, m_element_y, LANG_CHECKBOX_RECORD_KEYBIND, this, true);
	m_element_y += 40;
}

void dialog_new_element::add_element_id()
{
	if (m_element_y == 0)
		m_element_y = 30;
	add<label>(9, m_element_y, LANG_LABEL_ELEMENT_ID, this);
	m_element_y += 25;
	m_element_id = add<textbox>(8, m_element_y, panel_w, 20, ELEMENT_UNNAMED, this);
	m_element_id->set_flags(TEXTBOX_ALPHA_NUMERIC);
	m_element_y += 40;
}

void dialog_new_element::add_z_level()
{
	if (m_element_y == 0)
		m_element_y = 30;
	add<label>(9, m_element_y, LANG_LABEL_Z_LEVEL, this);
	m_element_y += 25;
	m_z_level = add<textbox>(8, m_element_y, panel_w, 20, "1", this);
	m_z_level->set_flags(TEXTBOX_NUMERIC);
	m_element_y += 40;
}

void dialog_new_element::add_mouse_or_analog_stick(const char *label_text, const char *item_a, const char *item_b)
{
	if (m_element_y == 0)
		m_element_y = 30;
	add<label>(9, m_element_y, label_text, this);
	m_element_y += 25;
	m_binary_choice = add<combobox>(8, m_element_y, panel_w, 20, this);
	m_binary_choice->add_item(item_a);
	m_binary_choice->add_item(item_b);
	m_element_y += 25;
	add<label>(9, m_element_y, LANG_LABEL_MOVEMENT_RADIUS, this);
	m_element_y += 25;
	m_radius = add<textbox>(8, m_element_y, panel_w, 20, "0", this);
	m_z_level->set_flags(TEXTBOX_NUMERIC);
	m_radius->set_flags(TEXTBOX_NUMERIC);
	m_element_y += 40;
}

void dialog_new_element::add_info(const char *unlocalized_text)
{
	if (m_element_y == 0)
		m_element_y = 30;
	auto *l = add<label>(9, m_element_y, unlocalized_text, this);
	m_element_y += 15 + l->get_height();
}

void dialog_new_element::add_trigger()
{
	if (m_element_y == 0)
		m_element_y = 30;
	add<label>(9, m_element_y, LANG_LABEL_TRIGGER_SIDE, this);
	m_element_y += 25;
	m_binary_choice = add<combobox>(8, m_element_y, panel_w, 20, this);
	m_binary_choice->add_item(LANG_LEFT);
	m_binary_choice->add_item(LANG_RIGHT);
	m_element_y += 25;

	add<label>(9, m_element_y, LANG_LABEL_TRIGGER_DIRECTION, this);
	m_element_y += 25;
	m_direction = add<combobox>(8, m_element_y, panel_w, 20, this);
	m_direction->add_item(LANG_UP);
	m_direction->add_item(LANG_DOWN);
	m_direction->add_item(LANG_LEFT);
	m_direction->add_item(LANG_RIGHT);
	m_element_y += 25;

	m_trigger_mode = add<checkbox>(8, m_element_y, LANG_CHECKBOX_TRIGGER_BUTON, this);
	m_element_y += 40;
}
