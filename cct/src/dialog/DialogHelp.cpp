#include "DialogHelp.hpp"

void DialogHelp::init()
{
    Dialog::init();
    add(new Label(1, 8, 32, LANG_LABEL_HELP_AND_ABOUT, this));

    add(new Button(ACTION_OK, 8, m_dimensions.h - 32, LANG_BUTTON_OK, this));
    add(new Button(2, 116, m_dimensions.h - 32, LANG_BUTTON_OPEN_URL, this));
    set_flags(DIALOG_CENTERED | DIALOG_TOP_MOST);
}

void DialogHelp::action_performed(const int8_t action_id)
{
    switch (action_id)
    {
    case ACTION_OK:
        if (m_tool)
            m_tool->queue_dialog_close();
        break;
    case 2:
        SDL_Helper::util_open_url("https://github.com/univrsal/input-overlay");
        break;
    default: ;
    }
}
