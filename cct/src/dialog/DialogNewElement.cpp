#include "DialogNewElement.hpp"

void DialogNewElement::init()
{
	Dialog::init();
	m_element_y = get_top() + 30;
	switch (m_type)
	{
	case BUTTON_KEYBOARD:
	case BUTTON_GAMEPAD:
	case BUTTON_MOUSE:

	case TEXTURE:
	
		add(m_selector = new AtlasSelector(m_id++, get_left() + 8,
			get_top() + 30, m_dimensions.w - 270, m_dimensions.h - 38, m_tool->get_atlas(), this));
		add_selection_elements();
		add_keycode_elements();
		break;
	}

	add(new Button(ACTION_OK, m_selector->get_width() + 16, m_dimensions.h - 32, "OK", this));
	add(new Button(ACTION_CANCEL, m_selector->get_width() + 124, m_dimensions.h - 32, "Cancel", this));
	set_flags(DIALOG_CENTERED | DIALOG_TOP_MOST);
}

void DialogNewElement::action_performed(int8_t action_id)
{
	switch (action_id)
	{
	case ACTION_OK:
		
		break;
	case ACTION_CANCEL:

		break;
	}
}

void DialogNewElement::draw_background(void)
{
	Dialog::draw_background();
	switch (m_type)
	{
	case TEXTURE:
	case BUTTON_KEYBOARD:
	case BUTTON_GAMEPAD:
	case MOUSE_MOVEMENT:

		break;
	}
}

void DialogNewElement::draw_foreground(void)
{
	Dialog::draw_foreground();
	switch (m_type)
	{
	case TEXTURE:
	case BUTTON_KEYBOARD:
	case BUTTON_GAMEPAD:
	case MOUSE_MOVEMENT:
	case ANALOG_STICK:
		break;
	}
}

bool DialogNewElement::handle_events(SDL_Event * event)
{
	bool was_handled = false;
	if (Dialog::handle_events(event))
		was_handled = true;

	switch (m_type)
	{
	case TEXTURE:
	case BUTTON_KEYBOARD:
	case BUTTON_GAMEPAD:
	case MOUSE_MOVEMENT:
	case ANALOG_STICK:
		break;
	}
	return was_handled;
}

void DialogNewElement::add_selection_elements(void)
{
	uint16_t panel_w = m_dimensions.w - m_selector->get_width() - 16;
	uint16_t left = m_selector->get_width();

	if (m_element_y == 0)
		m_element_y += 30;

	add(new Label(m_id++, left + 16, m_element_y, "Texture selection", this));

	m_element_y += 30;
	add(new Label(m_id++, left + 16, m_element_y, "Width:", this));
	add(new Label(m_id++, left + 8 + panel_w / 2, m_element_y, "Height:", this));
	add(m_w = new Textbox(m_id++, left + panel_w / 4 + 4, m_element_y, panel_w / 4, 20, "0", this));
	add(m_h = new Textbox(m_id++, left + panel_w / 4 * 3 + 4, m_element_y, panel_w / 4, 20, "0", this));

	m_element_y += 30;
	add(new Label(m_id++, left + 16, m_element_y, "X:", this));
	add(new Label(m_id++, left + 8 + panel_w / 2, m_element_y, "Y:", this));
	add(m_x = new Textbox(m_id++, left + panel_w / 4 + 4, m_element_y, panel_w / 4, 20, "0", this));
	add(m_y = new Textbox(m_id++, left + panel_w / 4 * 3 + 4, m_element_y, panel_w / 4, 20, "0", this));

	m_element_y += 30;
	add(new Label(m_id++, left + 16, m_element_y, "U:", this));
	add(new Label(m_id++, left + 8 + panel_w / 2, m_element_y, "V:", this));
	add(m_u = new Textbox(m_id++, left + panel_w / 4 + 4, m_element_y, panel_w / 4, 20, "0", this));
	add(m_v = new Textbox(m_id++, left + panel_w / 4 * 3 + 4, m_element_y, panel_w / 4, 20, "0", this));

	m_w->set_flags(TEXTBOX_NUMERIC); m_w->set_cutoff(20);
	m_h->set_flags(TEXTBOX_NUMERIC); m_h->set_cutoff(20);
	m_x->set_flags(TEXTBOX_NUMERIC); m_x->set_cutoff(20);
	m_y->set_flags(TEXTBOX_NUMERIC); m_y->set_cutoff(20);
	m_u->set_flags(TEXTBOX_NUMERIC); m_u->set_cutoff(20);
	m_v->set_flags(TEXTBOX_NUMERIC); m_v->set_cutoff(20);

	m_element_y += 40;
}

void DialogNewElement::add_keycode_elements(void)
{
	uint16_t panel_w = m_dimensions.w - m_selector->get_width() - 16;
	uint16_t left = m_selector->get_width();

	if (m_element_y == 0)
		m_element_y += 30;
	add(new Label(m_id++, left + 16, m_element_y, "Keycode:", this));
	m_element_y += 30;
	add(m_keycode = new Textbox(m_id++, left + 16, m_element_y, panel_w - 8, 20, "0", this));
	m_keycode->set_cutoff(10);
	m_element_y += 40;
}
