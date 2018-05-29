#include "Tool.hpp"

Tool::Tool(SDL_helper * helper)
{
	m_helper = helper;
}

Tool::~Tool()
{
	m_helper = nullptr;
	if (m_setup_dialog)
		delete m_setup_dialog;

	if (m_config)
		delete m_config;

	m_config = nullptr;
	m_setup_dialog = nullptr;
}

void Tool::program_loop()
{
	m_setup_dialog = new DialogSetup(m_helper, SDL_Point{ 500, 230 }, "Overlay setup");
	m_setup_dialog->init();
	m_setup_dialog->action_performed(ACTION_FOCUSED);
	m_helper->set_runflag(&m_run_flag);

	bool in_setup = true;

	while (m_run_flag)
	{
		handle_input();
		m_helper->clear();
		// Drawing

		if (in_setup)
		{
			m_setup_dialog->draw_background();
			m_setup_dialog->draw_foreground();

			if (m_setup_dialog->is_finished())
			{
				in_setup = false;
				m_element_settings = new DialogElementSettings(m_helper, SDL_Point{ 240, 500 },
					"Selected element settings", this);
				m_config = new Config(m_setup_dialog->get_texture_path(),
					m_setup_dialog->get_config_path(), m_helper, m_element_settings);


				m_element_settings->init();

				
				/* Free it up */
				delete m_setup_dialog;
				m_setup_dialog = nullptr;
				
				m_focused_dialog = m_element_settings;
				m_state = IN_BUILD;
			}
		}
		else
		{
			m_config->draw_elements();

			m_focused_dialog->draw_background();
			m_focused_dialog->draw_foreground();
		}
		m_helper->repaint();
	}
}

Element * Tool::get_selected(void)
{
	if (m_config)
		return m_config->m_selected;
	return nullptr;
}

void Tool::handle_input()
{
	while (SDL_PollEvent(&m_event) != 0)
	{
		if (m_event.type == SDL_QUIT)
		{
			m_run_flag = false;
		}

		m_helper->handle_events(&m_event);

		switch (m_state)
		{
		case IN_SETUP:
			m_setup_dialog->handle_events(&m_event);
			break;
		case IN_BUILD:
			if (m_focused_dialog && !m_focused_dialog->handle_events(&m_event))
			{
				/* Focused dialog didn't handle the event */
				if (m_config)
					m_config->handle_events(&m_event);
			}
			break;
		case IN_NEW_ELEMENT:

			break;
		}

	}
}
