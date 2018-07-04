/**
 * Created by univrsal on 23.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "DialogSetup.hpp"
#include "../util/Notifier.hpp"

void DialogSetup::init()
{
	Dialog::init();
	int8_t id = 1;

	add(new Label(id++, 8, 35, "Enter the path to the texture file:", this));
	add(m_texture_path = new Textbox(id++, 8, 55, m_dimensions.w - 16, 20, "D:\\Projects\\prog\\cpp\\input-overlay-releases\\build\\v4.6-pre\\presets\\\wasd-full\\wasd.png", this));

	add(new Label(id++, 8, 85, "Default element width:", this));
	add(new Label(id++, (m_dimensions.w / 2) + 4, 85, "Default element height:", this));

	m_def_w = new Textbox(id++, 8, 105, (m_dimensions.w / 2) - 16, 20, "0", this);
	m_def_h = new Textbox(id++, (m_dimensions.w / 2) + 4, 105, (m_dimensions.w / 2) - 12, 20, "0", this);

	m_def_w->set_flags(TEXTBOX_NUMERIC);
	m_def_h->set_flags(TEXTBOX_NUMERIC);

	add(m_def_w);
	add(m_def_h);

	add(new Button(ACTION_OK, 8, m_dimensions.h - 32, "OK", this));
	add(new Button(ACTION_CANCEL, 116, m_dimensions.h - 32, "Exit", this));

	add(new Label(id++, 8, 135, "Enter config path for saving or loading:", this));
	add(m_config_path = new Textbox(id++, 8, 155, m_dimensions.w - 16, 20, "C:\\Users\\usr\\Desktop\\test.ini", this));

	set_flags(DIALOG_CENTERED | DIALOG_TEXTINPUT);
}

void DialogSetup::action_performed(int8_t action_id)
{
	bool valid_texture;
	bool valid_config;
	ccl_config cfg;
	
	switch (action_id)
	{
	case ACTION_OK:
		valid_texture = m_helper->util_check_texture_path(m_texture_path->get_text()->c_str());
		cfg = ccl_config(*m_config_path->get_text(), "");
		valid_config = cfg.can_write();
		cfg.free();

		if (!m_texture_path->get_text()->empty() && !m_config_path->get_text()->empty() && valid_texture && valid_config)
		{
			m_finished = true;
		}
		else
		{
			if (m_texture_path->get_text()->empty() || !valid_texture)
			{
				m_texture_path->set_alert(true);
				m_notifier->add_msg(MESSAGE_ERROR, "Invalid texture path!");
			}
				
			if (m_config_path->get_text()->empty() || !valid_config)
			{
				m_config_path->set_alert(true);
				m_notifier->add_msg(MESSAGE_ERROR, "Invalid config path!");
			}
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

SDL_Point DialogSetup::get_default_dim()
{
	return SDL_Point{ std::stoi(m_def_w->get_text()->c_str()),
		std::stoi(m_def_h->get_text()->c_str()) };
}
