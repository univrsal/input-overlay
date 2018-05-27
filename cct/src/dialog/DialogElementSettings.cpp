#include "DialogElementSettings.hpp"

void DialogElementSettings::init()
{
	Dialog::init();
	int8_t id = 1;

	/* Labels */
	add(new Label(id++, 8, 35, "Width: ", this));
	add(new Label(id++, (m_dimensions.w / 2) + 4, 35, "Height: ", this));
	add(new Label(id++, 8, 65, "X: ", this));
	add(new Label(id++, (m_dimensions.w / 2) + 4, 65, "Y: ", this));
	add(new Label(id++, 8, 95, "U: ", this));
	add(new Label(id++, (m_dimensions.w / 2) + 4, 95, "V: ", this));

	/* Textboxes */
	m_element_width = new Textbox(id++, 55, 32, (m_dimensions.w / 2) - 63, 20, "0", this);
	m_element_height = new Textbox(id++, (m_dimensions.w / 2) + 55, 32, (m_dimensions.w / 2) - 63, 20, "0", this);

	m_element_x = new Textbox(id++, 55, 62, (m_dimensions.w / 2) - 63, 20, "0", this);
	m_element_y = new Textbox(id++, (m_dimensions.w / 2) + 55, 62, (m_dimensions.w / 2) - 63, 20, "0", this);

	m_element_u = new Textbox(id++, 55, 92, (m_dimensions.w / 2) - 63, 20, "0", this);
	m_element_v = new Textbox(id++, (m_dimensions.w / 2) + 55, 92, (m_dimensions.w / 2) - 63, 20, "0", this);

	m_element_width->set_flags(TEXTBOX_NUMERIC);
	m_element_height->set_flags(TEXTBOX_NUMERIC);
	m_element_x->set_flags(TEXTBOX_NUMERIC);
	m_element_y->set_flags(TEXTBOX_NUMERIC);
	m_element_u->set_flags(TEXTBOX_NUMERIC);
	m_element_v->set_flags(TEXTBOX_NUMERIC);

	add(m_element_width);
	add(m_element_height);
	add(m_element_x);
	add(m_element_y);
	add(m_element_u);
	add(m_element_v);

	add(new Button(ACTION_OK, 8, m_dimensions.h - 32, "OK", this));

	set_flags(DIALOG_DRAGGABLE | DIALOG_TEXTINPUT);
}

void DialogElementSettings::action_performed(int8_t action_id)
{
}
