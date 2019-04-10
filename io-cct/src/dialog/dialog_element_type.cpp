/**
 * Created by univrsal on 15.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#include "dialog_element_type.hpp"
#include "elements/combobox.hpp"
#include "elements/button.hpp"
#include "../tool.hpp"

DialogElementType::DialogElementType(sdl_helper* sdl, tool* tool) : dialog(sdl, SDL_Point{350, 120},
                                                                           LANG_DIALOG_ELEMENT_TYPE)
{
    m_tool = tool;
}

void DialogElementType::init()
{
    dialog::init();
    add(new label(1, 8, 32, LANG_LABEL_SELECT_ELEMENT_TYPE, this));

    add(m_type = new combobox(2, 8, 52, m_dimensions.w - 16, 20, this));

    /* Populate combobox with element types */
    m_type->add_item(LANG_ELEMENT_TEXTURE);
    m_type->add_item(LANG_ELEMENT_BUTTON);
    m_type->add_item(LANG_ELEMENT_MOUSE_WHEEL);
    m_type->add_item(LANG_ELEMENT_MOUSE_MOVEMENT);
    m_type->add_item(LANG_ELEMENT_ANALOG_STICK);
    m_type->add_item(LANG_ELEMENT_TRIGGER);
    m_type->add_item(LANG_ELEMENT_GAMEPAD_ID);
    m_type->add_item(LANG_ELEMENT_DPAD_STICK);

    add(new button(ACTION_OK, 8, m_dimensions.h - 32, LANG_BUTTON_OK, this));
    add(new button(ACTION_CANCEL, 124, m_dimensions.h - 32, LANG_BUTTON_CANCEL, this));

    set_flags(DIALOG_CENTERED | DIALOG_TOP_MOST);
}

void DialogElementType::action_performed(const int8_t action_id)
{
    switch (action_id) {
        case ACTION_CANCEL:
            m_tool->queue_dialog_close();
            break;
        case ACTION_OK:
            m_tool->set_new_element_type(static_cast<element_type>(m_type->get_selected()));
            m_tool->queue_dialog_open(NEW_ELEMENT);
            break;
        default:;
    }
}
