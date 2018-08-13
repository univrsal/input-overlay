#include "DialogHelp.hpp"

void DialogHelp::init()
{
	Dialog::init();
	add(new Label(1, 8, 32, LANG_LABEL_HELP_AND_ABOUT, this));

	add(new Button(ACTION_OK, 8, m_dimensions.h - 32, LANG_BUTTON_OK, this));

	set_flags(DIALOG_CENTERED | DIALOG_TOP_MOST);
}

void DialogHelp::action_performed(int8_t action_id)
{
	if (m_tool)
		m_tool->action_performed(TOOL_ACTION_HELP_EXIT);
}
