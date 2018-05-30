#include "Tool.hpp"

Tool::Tool(SDL_helper * helper)
{
	m_helper = helper;
	m_state = IN_SETUP;
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

	while (m_run_flag)
	{
		handle_input();
		m_helper->clear();

		// Drawing
		switch (m_state)
		{
			case IN_SETUP:
				m_setup_dialog->draw_background();
				m_setup_dialog->draw_foreground();

				if (m_setup_dialog->is_finished())
				{
					m_element_settings = new DialogElementSettings(m_helper, SDL_Rect { 10, 200, 240, 310 },
						"Selected element settings", this);
					m_config = new Config(m_setup_dialog->get_texture_path(),
						m_setup_dialog->get_config_path(), m_helper, m_element_settings);

					m_element_settings->init();
					
					/* Free it up */
					delete m_setup_dialog;
					m_setup_dialog = nullptr;
					m_state = IN_BUILD;
				}
			break;
			case IN_BUILD:
				m_config->draw_elements();
				m_element_settings->draw_background();
				m_element_settings->draw_foreground();
			break;
			case IN_HELP:
				m_config->draw_elements();
				m_element_settings->draw_background();
				m_element_settings->draw_foreground();
				if (m_toplevel)
				{
					m_toplevel->draw_background();
					m_toplevel->draw_foreground();
				}
			break;
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

void Tool::action_performed(uint8_t type)
{
	switch (type)
	{
		case TOOL_ACTION_HELP_OPEN:
			close_toplevel();
			m_state = IN_HELP;
			m_toplevel = new DialogHelp(m_helper, SDL_Point { 350, 370 }, "Help and about", this);
			m_toplevel->init();
		break;
		case TOOL_ACTION_HELP_EXIT:
			m_state = IN_BUILD;
			m_queue_close = true;
		break;
	}
}

void Tool::close_toplevel(void)
{
	if (m_toplevel)
	{
		delete m_toplevel;
		m_toplevel = nullptr;
	}
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
			if (m_element_settings && !m_element_settings->handle_events(&m_event))
			{
				/* Focused dialog didn't handle the event */
				if (m_config)
					m_config->handle_events(&m_event);
			}
			break;
		case IN_HELP:
			if (m_toplevel)
				m_toplevel->handle_events(&m_event);
			break;
		case IN_NEW_ELEMENT:

			break;
		}
	}

	if (m_queue_close)
	{
		m_queue_close = false;
		close_toplevel();
	}
}
