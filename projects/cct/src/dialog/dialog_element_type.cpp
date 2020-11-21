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

#include "dialog_element_type.hpp"
#include "../tool.hpp"
#include "../util/constants.hpp"
#include "elements/button.hpp"
#include "elements/combobox.hpp"

DialogElementType::DialogElementType(sdl_helper *sdl, tool *tool)
	: dialog(sdl, SDL_Point{350, 120}, LANG_DIALOG_ELEMENT_TYPE)
{
	m_tool = tool;
}

void DialogElementType::init()
{
	dialog::init();
	add<label>(8, 32, LANG_LABEL_SELECT_ELEMENT_TYPE, this);

	m_type = add<combobox>(8, 52, m_dimensions.w - 16, 20, this);

	/* Populate combobox with element types */
	m_type->add_item(LANG_ELEMENT_TEXTURE);
	m_type->add_item(LANG_ELEMENT_BUTTON);
	m_type->add_item(LANG_ELEMENT_MOUSE_WHEEL);
	m_type->add_item(LANG_ELEMENT_MOUSE_MOVEMENT);
	m_type->add_item(LANG_ELEMENT_ANALOG_STICK);
	m_type->add_item(LANG_ELEMENT_TRIGGER);
	m_type->add_item(LANG_ELEMENT_GAMEPAD_ID);
	m_type->add_item(LANG_ELEMENT_DPAD_STICK);

	add<button>(8, m_dimensions.h - 32, LANG_BUTTON_OK, this)->set_id(ACTION_OK);
	add<button>(124, m_dimensions.h - 32, LANG_BUTTON_CANCEL, this)->set_id(ACTION_CANCEL);

	set_flags(DIALOG_CENTERED | DIALOG_TOP_MOST);
}

void DialogElementType::action_performed(const int8_t action_id)
{
	switch (action_id) {
	case ACTION_CANCEL:
		m_tool->queue_dialog_close();
		break;
	case ACTION_OK:
		m_tool->set_new_element_type(static_cast<element_type>(m_type->get_selected()));
		m_tool->queue_dialog_open(NEW_ELEMENT);
		break;
	default:;
	}
}
