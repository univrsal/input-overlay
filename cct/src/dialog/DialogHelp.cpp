#include "DialogHelp.hpp"

void DialogHelp::init()
{
	Dialog::init();
	add(new Label(1, 8, 32, "input-overlay config creation tool\nlicensed under the Mozilla Public Licence 2.0\ngithub.com/univrsal/input-overlay\n\nThanks to:\nThe OBS Studio team and contributers\nobsproject.com\nThe SDL developers\nlibsdl.org\n\nUsage:\nScroll wheel: Zoom in/out\nLeft mouse: move elements\nRight mouse: move preview\n\nSome keys will work within the preview\nand light up when pressed", this));

	add(new Button(ACTION_OK, 8, m_dimensions.h - 32, "OK", this));

	set_flags(DIALOG_CENTERED | DIALOG_TOP_MOST);
}

void DialogHelp::action_performed(int8_t action_id)
{
	if (m_tool)
		m_tool->action_performed(TOOL_ACTION_HELP_EXIT);
}
