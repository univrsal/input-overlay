#include "Tool.hpp"

Tool::Tool(SDL_helper * helper)
{
	m_helper = helper;
}

Tool::~Tool()
{
	m_helper = nullptr;
	delete m_setup_dialog;
	m_setup_dialog = nullptr;
}

void Tool::program_loop()
{
	m_setup_dialog = new DialogSetup(m_helper, SDL_Point { 500, 180 }, "Overlay setup");
	m_setup_dialog->init();
	m_setup_dialog->action_performed(ACTION_FOCUSED);

	while (m_run_flag)
	{
		handle_input();

		m_helper->clear();
		// Drawing
		m_setup_dialog->draw_background();
		m_setup_dialog->draw_foreground();
		m_helper->repaint();
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
		m_setup_dialog->handle_events(&m_event);
	}
}
