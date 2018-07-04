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
	close_toplevel();
	m_config = nullptr;
	m_setup_dialog = nullptr;
	if (m_notify)
	{
		delete m_notify;
		m_notify = nullptr;
	}
}

void Tool::program_loop()
{
	m_notify = new Notifier(m_helper);

	m_setup_dialog = new DialogSetup(m_helper, SDL_Point{ 500, 230 }, m_notify);
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
				m_element_settings = new DialogElementSettings(m_helper,
					SDL_Rect{ 10, 200, 240, 360 }, this);
				m_config = new Config(m_setup_dialog->get_texture_path(),
					m_setup_dialog->get_config_path(), m_setup_dialog->get_default_dim(), m_helper, m_element_settings);

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
		case IN_NEW_ELEMENT:
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
		m_notify->draw(); /* Notifications have top priority */
		m_helper->repaint();
	}
}

Element * Tool::get_selected(void)
{
	if (m_config)
		return m_config->selected();
	return nullptr;
}

uint16_t Tool::get_selected_id(void)
{
	return m_config->selecte_id();
}

void Tool::action_performed(uint8_t type)
{
	DialogNewElement * d = nullptr;
	Element * e = nullptr;

	switch (type)
	{
	case TOOL_ACTION_HELP_OPEN:
		close_toplevel();
		m_state = IN_HELP;
		m_toplevel = new DialogHelp(m_helper, SDL_Point{ 350, 420 }, this);
		m_toplevel->init();
		break;
	case TOOL_ACTION_HELP_EXIT:
		m_state = IN_BUILD;
		m_queue_close = true;
		break;
	case TOOL_ACTION_NEW_ELEMENT_OPEN:
		close_toplevel();
		m_state = IN_NEW_ELEMENT;
		m_toplevel = new DialogNewElement(m_helper, SDL_Point{}, "New element", this, BUTTON_KEYBOARD);
		m_toplevel->init();
		d = reinterpret_cast<DialogNewElement*>(m_toplevel);
		d->set_default_dim(m_config->get_default_dim().x, m_config->get_default_dim().y);
		break;
	case TOOL_ACTION_NEW_ELEMENT_ADD:
		d = reinterpret_cast<DialogNewElement*>(m_toplevel);
		
		switch (d->get_type())
		{

		case TEXTURE:
		case BUTTON_KEYBOARD:
		case BUTTON_GAMEPAD:
		case MOUSE_MOVEMENT:
		case ANALOG_STICK:
		case BUTTON_MOUSE:
			e = new Element(d->get_type(), *d->get_id(), SDL_Point{ 0, 0 }, d->get_selection_1(), d->get_key_code());
			break;
		case TRIGGER_GAMEPAD:
			break;
		case TEXT:
			break;
		case DPAD:
			break;
		case DPAD_STICK:
			break;	
		}

		if (e)
			add_element(e);
		m_queue_close = true;
		m_state = IN_BUILD;
		break;
	case TOOL_ACTION_NEW_ELEMENT_EXIT:
		m_queue_close = true;
		m_state = IN_BUILD;
		break;
	}
}

Texture * Tool::get_atlas(void)
{
	return m_config->get_texture();
}

void Tool::delete_element(uint16_t id)
{
	m_config->queue_delete(id);
}

void Tool::add_element(Element * e)
{
	bool can_add = true;
	for (auto& const element : m_config->m_elements)
	{
		if (element->get_id()->compare(e->get_id()->c_str()) == 0)
		{
			can_add = false;
			m_notify->add_msg(MESSAGE_ERROR, "Element id must be unique!");
			break;
		}
	}

	if (e->get_vc() == 0)
	{
		m_notify->add_msg(MESSAGE_INFO, "Key code 0x0 is invalid");
	}

	if (SDL_RectEmpty(e->get_mapping()))
	{
		m_notify->add_msg(MESSAGE_ERROR, "Selection was empty!");
		can_add = false;
	}

	if (can_add)
	{
		m_config->m_elements.emplace_back(e);
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
			if (m_event.type == SDL_WINDOWEVENT
				&& m_event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				m_config->handle_events(&m_event); /* Still need it to resize*/
			}
			break;
		case IN_NEW_ELEMENT:
			if (m_toplevel)
			{
				m_toplevel->handle_events(&m_event);
				if (m_event.type == SDL_WINDOWEVENT
					&& m_event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					m_config->handle_events(&m_event); /* Still need it to resize*/
				}
			}
			break;
		}
	}

	if (m_queue_close)
	{
		m_queue_close = false;
		close_toplevel();
	}
}
