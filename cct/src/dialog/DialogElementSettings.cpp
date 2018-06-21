#include "DialogElementSettings.hpp"
#include "../Config.hpp"

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
	add(new Label(id++, 8, 125, "Element id:", this));

	/* Textboxes */
	m_element_width = new Textbox(id++, 55, 32, (m_dimensions.w / 2) - 63, 20, "0", this);
	m_element_height = new Textbox(id++, (m_dimensions.w / 2) + 55, 32, (m_dimensions.w / 2) - 63, 20, "0", this);

	m_element_x = new Textbox(id++, 55, 62, (m_dimensions.w / 2) - 63, 20, "0", this);
	m_element_y = new Textbox(id++, (m_dimensions.w / 2) + 55, 62, (m_dimensions.w / 2) - 63, 20, "0", this);

	m_element_u = new Textbox(id++, 55, 92, (m_dimensions.w / 2) - 63, 20, "0", this);
	m_element_v = new Textbox(id++, (m_dimensions.w / 2) + 55, 92, (m_dimensions.w / 2) - 63, 20, "0", this);

	add(m_element_id = new Textbox(id++, 8, 145, m_dimensions.w - 16, 20, "", this));

	m_element_width->set_flags(TEXTBOX_NUMERIC);
	m_element_height->set_flags(TEXTBOX_NUMERIC);
	m_element_x->set_flags(TEXTBOX_NUMERIC);
	m_element_y->set_flags(TEXTBOX_NUMERIC);
	m_element_u->set_flags(TEXTBOX_NUMERIC);
	m_element_v->set_flags(TEXTBOX_NUMERIC);

	m_element_width->set_cutoff(20);
	m_element_height->set_cutoff(20);
	m_element_x->set_cutoff(20);
	m_element_y->set_cutoff(20);
	m_element_u->set_cutoff(20);
	m_element_v->set_cutoff(20);

	add(m_element_width);
	add(m_element_height);
	add(m_element_x);
	add(m_element_y);
	add(m_element_u);
	add(m_element_v);

	add(new Button(ACTION_NEW_ELEMENT, 8, m_dimensions.h - 154, m_dimensions.w - 16, "Add new element", this));
	add(new Button(ACTION_DEL_ELEMENT, 8, m_dimensions.h - 126, m_dimensions.w - 16, "Delete selected", this));
	add(new Button(ACTION_SAVE_CONFIG, 8, m_dimensions.h - 98, m_dimensions.w - 16, "Save config", this));
	add(new Button(ACTION_HELP_BUTTON, 8, m_dimensions.h - 70, m_dimensions.w - 16, "Help", this));
	add(new Button(ACTION_OK, 8, m_dimensions.h - 32, "OK", this));

	set_flags(DIALOG_DRAGGABLE | DIALOG_TEXTINPUT);
}

void DialogElementSettings::action_performed(int8_t action_id)
{
	switch (action_id)
	{
	case ACTION_OK:
		if (m_tool->get_selected())
		{
			m_tool->get_selected()->set_pos(std::stoi(
				m_element_x->get_text()->c_str()), std::stoi(
					m_element_y->get_text()->c_str()));
			m_tool->get_selected()->set_uv(std::stoi(
				m_element_u->get_text()->c_str()), std::stoi(
					m_element_v->get_text()->c_str()));
			m_tool->get_selected()->set_dim(std::stoi(
				m_element_width->get_text()->c_str()), std::stoi(
					m_element_height->get_text()->c_str()));
			m_tool->get_selected()->set_id(*m_element_id->get_text());
		}
		break;
	case ACTION_DEL_ELEMENT:
		if (m_tool->get_selected())
		{
			m_tool->delete_element(m_tool->get_selected_id());
		}
		break;
	case ACTION_HELP_BUTTON:
		m_tool->action_performed(TOOL_ACTION_HELP_OPEN);
		break;
	case ACTION_NEW_ELEMENT:
		m_tool->action_performed(TOOL_ACTION_NEW_ELEMENT_OPEN);
		break;
	}
}

void DialogElementSettings::set_wh(int w, int h)
{
	if (w >= 0)
	{
		m_element_width->set_text(std::to_string(w));
	}

	if (h >= 0)
	{
		m_element_height->set_text(std::to_string(h));
	}
}

void DialogElementSettings::set_xy(int x, int y)
{
	if (x >= 0)
	{
		m_element_x->set_text(std::to_string(x));
	}

	if (y >= 0)
	{
		m_element_y->set_text(std::to_string(y));
	}
}

void DialogElementSettings::set_uv(int u, int v)
{
	if (u >= 0)
	{
		m_element_u->set_text(std::to_string(u));
	}

	if (v >= 0)
	{
		m_element_v->set_text(std::to_string(v));
	}
}

void DialogElementSettings::set_id(std::string id)
{
	m_element_id->set_text(id);
}

void DialogElementSettings::select_element(Element * e)
{
	if (e)
	{
		set_id(*e->get_id());
		set_uv(e->get_u(), e->get_v());
		set_xy(e->get_x(), e->get_y());
		set_wh(e->get_w(), e->get_h());
	}
}
