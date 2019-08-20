#include "dialog_help.hpp"

void DialogHelp::init()
{
	dialog::init();
	const auto info = new label(1, 8, 32, LANG_LABEL_HELP_AND_ABOUT, this);
	add(info);

	add(new button(ACTION_OK, 8, m_dimensions.h - 32, LANG_BUTTON_OK, this));
	add(new button(2, 116, m_dimensions.h - 32, LANG_BUTTON_OPEN_URL, this));
	set_flags(DIALOG_CENTERED | DIALOG_TOP_MOST);

	const auto temp = m_dimensions;
	set_dimension(UTIL_MAX(temp.w, info->get_width() + 16), temp.h);
}

void DialogHelp::action_performed(const int8_t action_id)
{
	switch (action_id) {
	case ACTION_OK:
		if (m_tool)
			m_tool->queue_dialog_close();
		break;
	case 2:
		sdl_helper::util_open_url("https://github.com/univrsal/input-overlay");
		break;
	default: ;
	}
}
