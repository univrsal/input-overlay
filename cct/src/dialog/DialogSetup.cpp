/**
 * Created by universallp on 23.05.2018.
 * This file is part of input-overlay which is licenced
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "DialogSetup.hpp"

void DialogSetup::init()
{
	Dialog::init();
	int8_t id = 1;

	m_screen_elements.emplace_back(new Label(id++, 8, 35, "Enter the path to the texture file:", this));
	m_screen_elements.emplace_back(m_texture_path = new Textbox(id++, 8, 55, m_dimensions.w - 16, 20, "", this));

	m_screen_elements.emplace_back(new Label(id++, 8, 85, "Default element width:", this));
	m_screen_elements.emplace_back(new Label(id++, (m_dimensions.w / 2) + 4, 85, "Default element height:", this));

	Textbox * def_w = new Textbox(id++, 8, 105, (m_dimensions.w / 2) - 16, 20, "0", this);
	Textbox * def_h = new Textbox(id++, (m_dimensions.w / 2) + 4, 105, (m_dimensions.w / 2) - 12, 20, "0", this);

	def_w->set_flags(TEXTBOX_NUMERIC);
	def_h->set_flags(TEXTBOX_NUMERIC);

	m_screen_elements.emplace_back(def_w);
	m_screen_elements.emplace_back(def_h);

	m_screen_elements.emplace_back(new Button(ACTION_OK, 8, m_dimensions.h - 32, "OK", this));
	m_screen_elements.emplace_back(new Button(ACTION_CANCEL, 116, m_dimensions.h - 32, "Exit", this));

	m_screen_elements.emplace_back(new Label(id++, 8, 135, "Enter path to config to edit or open it:", this));
	m_screen_elements.emplace_back(m_config_path = new Textbox(id++, 8, 155, m_dimensions.w - 16, 20, "", this));

	set_flags(DIALOG_CENTERED | DIALOG_TEXTINPUT);
}

void DialogSetup::action_performed(int8_t action_id)
{
	switch (action_id)
	{
	case ACTION_OK:
		if (!m_texture_path->get_text()->empty() && !m_config_path->get_text()->empty())
		{
			m_finished = true;
		}
		else
		{
			if (m_texture_path->get_text()->empty())
				m_texture_path->set_alert(true);

			if (m_config_path->get_text()->empty())
				m_config_path->set_alert(true);
		}
		break;
	case ACTION_CANCEL:
		m_helper->exit_loop();
		break;
	}
}

bool DialogSetup::is_finished(void)
{
	return m_finished;
}

const std::string * DialogSetup::get_config_path(void)
{
	return m_config_path->get_text();
}

const std::string * DialogSetup::get_texture_path(void)
{
	return m_texture_path->get_text();
}
