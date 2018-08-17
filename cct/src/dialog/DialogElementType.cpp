/**
 * Created by univrsal on 15.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "DialogElementType.hpp"
#include "elements/Combobox.hpp"
#include "../Tool.hpp"

DialogElementType::DialogElementType(SDL_helper * sdl, Tool * tool)
	: Dialog(sdl, SDL_Point { 350, 120 }, LANG_DIALOG_ELEMENT_TYPE)
{
	m_tool = tool;
}

void DialogElementType::init()
{
	Dialog::init();
	add(new Label(1, 8, 32, LANG_LABEL_SELECT_ELEMENT_TYPE, this));
	
	add(new Button(ACTION_OK, 8, m_dimensions.h - 32, LANG_BUTTON_OK, this));
	add(new Button(ACTION_CANCEL, 124, m_dimensions.h - 32, LANG_BUTTON_CANCEL, this));
	add(m_type = new Combobox(2, 8, 52, m_dimensions.w - 16, 20, this));
	
	/* Populate combobox with element types */
	m_type->add_item(LANG_ELEMENT_TEXTURE);
	m_type->add_item(LANG_ELEMENT_BUTTON);
	m_type->add_item(LANG_ELEMENT_MOUSE_WHEEL);
	m_type->add_item(LANG_ELEMENT_MOUSE_MOVEMENT);
	m_type->add_item(LANG_ELEMENT_ANALOG_STICK);
	m_type->add_item(LANG_ELEMENT_TRIGGER);
	m_type->add_item(LANG_ELEMENT_TEXT);
	m_type->add_item(LANG_ELEMENT_DPAD_STICK);

	set_flags(DIALOG_CENTERED | DIALOG_TOP_MOST);
}

void DialogElementType::action_performed(int8_t action_id)
{
	switch (action_id)
	{
	case ACTION_CANCEL:
		m_tool->queue_dialog_close();
		break;
	case ACTION_OK:
		m_tool->set_new_element_type((ElementType) m_type->get_selected());
		m_tool->queue_dialog_open(DialogID::NEW_ELEMENT);
		break;
	}
}
