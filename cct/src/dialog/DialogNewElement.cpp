#include "DialogNewElement.hpp"
#include "../Config.hpp"

void DialogNewElement::close(void)
{

}

void DialogNewElement::load_from_element(Element * e)
{
    if (e)
    {
        m_u->set_text(std::to_string(e->get_u()));
        m_v->set_text(std::to_string(e->get_v()));

        m_w->set_text(std::to_string(e->get_w()));
        m_h->set_text(std::to_string(e->get_h()));

        m_element_id->set_text(*e->get_id());

        if (m_keycode)
            m_keycode->set_hex_int(e->get_vc());

        m_selection.x = e->get_u();
        m_selection.y = e->get_v();
        m_selection.w = e->get_w();
        m_selection.h = e->get_h();

        m_modify_mode = true;
    }
}

void DialogNewElement::init()
{
    set_flags(DIALOG_TOP_MOST | DIALOG_FLUID);
    Dialog::init();
    m_element_y = get_top() + 30;

    add_element_id();
    add_z_level();

    switch (m_type)
    {
    case BUTTON:
        add_keycode_elements();
    case TEXTURE:
        add(m_selector = new AtlasSelector(m_id++, get_left() + 270,
            get_top() + 30, m_dimensions.w - 278, m_dimensions.h - 38, m_tool->get_atlas(), this));
        m_selector->set_selection(&m_selection);
        add_selection_elements();
        break;
    }

    add(m_ok = new Button(ACTION_OK, 8, m_dimensions.h - 32, LANG_BUTTON_OK, this));
    add(m_cancel = new Button(ACTION_CANCEL, 124, m_dimensions.h - 32, LANG_BUTTON_CANCEL, this));
}

void DialogNewElement::action_performed(int8_t action_id)
{
    switch (action_id)
    {
    case ACTION_OK:
        if (m_modify_mode)
            m_tool->action_performed(TOOL_ACTION_MOD_ELEMENT_APPLY);
        else
            m_tool->action_performed(TOOL_ACTION_NEW_ELEMENT_ADD);
        break;
    case ACTION_CANCEL:
        m_tool->queue_dialog_close();
        break;
    default:
        if (m_read_keybind->get_state())
            m_keycode->set_flags(TEXTBOX_HEX | TEXTBOX_KEYBIND);
        else
            m_keycode->set_flags(TEXTBOX_HEX);
    }
}

void DialogNewElement::draw_background(void)
{
    Dialog::draw_background();
    switch (m_type)
    {
    case TEXTURE:
    case BUTTON:
    case MOUSE_MOVEMENT:

        break;
    }
}

void DialogNewElement::draw_foreground(void)
{
    Dialog::draw_foreground();
    switch (m_type)
    {
    case TEXTURE:
    case BUTTON:
    case MOUSE_MOVEMENT:
    case ANALOG_STICK:
        break;
    }
}

bool DialogNewElement::handle_events(SDL_Event * event)
{
    bool was_handled = false;
    if (Dialog::handle_events(event))
        was_handled = true;

    switch (m_type)
    {
    case TEXTURE:
    case BUTTON:
    case MOUSE_MOVEMENT:
    case ANALOG_STICK:
        if (event->type == SDL_WINDOWEVENT)
        {
            if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                SDL_Rect t = { get_left() + 270,
                    get_top() + 30, m_dimensions.w - 278, m_dimensions.h - 38 };
                m_selector->set_dim(t);
                m_ok->set_pos(8, m_dimensions.h - 32);
                m_cancel->set_pos(124, m_dimensions.h - 32);
            }
        }
        m_h->set_text(std::to_string(m_selection.h));
        m_w->set_text(std::to_string(m_selection.w));
        m_u->set_text(std::to_string(m_selection.x));
        m_v->set_text(std::to_string(m_selection.y));
        break;
    }

    return was_handled;
}

ElementType DialogNewElement::get_type(void)
{
    return m_type;
}

SDL_Rect DialogNewElement::get_selection(void)
{
    return m_selection;
}

uint16_t DialogNewElement::get_vc(void)
{
    uint16_t t = 0x0;

    if (m_keycode)
    {
        t = (uint16_t) strtoul(m_keycode->get_text()->c_str(), nullptr, 16);
    }

    return t;
}

uint8_t DialogNewElement::get_z_level(void)
{
    return atoi(m_z_level->get_text()->c_str());
}

const std::string * DialogNewElement::get_id(void)
{
    return m_element_id->get_text();
}

void DialogNewElement::set_default_dim(int w, int h)
{
    m_selection.w = w;
    m_selection.h = h;

    m_selection.x = 1;
    m_selection.y = 1;
}

void DialogNewElement::add_selection_elements(void)
{
    uint16_t panel_w = 254;

    m_selection = { 0, 0, 0, 0 };

    if (m_element_y == 0)
        m_element_y = 30;

    add(new Label(m_id++, 8, m_element_y, LANG_LABEL_TEXTURE_SELECTION, this));

    m_element_y += 25;
    add(new Label(m_id++, 8, m_element_y, LANG_LABEL_WIDTH, this));
    add(new Label(m_id++, 16 + panel_w / 2, m_element_y, LANG_LABEL_HEIGHT, this));
    add(m_w = new Textbox(m_id++, 8 + panel_w / 4 + 4, m_element_y, panel_w / 4, 20, "0", this));
    add(m_h = new Textbox(m_id++, 8 + panel_w / 4 * 3 + 4, m_element_y, panel_w / 4, 20, "0", this));

    m_element_y += 25;
    add(new Label(m_id++, 8, m_element_y, LANG_LABEL_U, this));
    add(new Label(m_id++, 16 + panel_w / 2, m_element_y, LANG_LABEL_V, this));
    add(m_u = new Textbox(m_id++, 8 + panel_w / 4 + 4, m_element_y, panel_w / 4, 20, "0", this));
    add(m_v = new Textbox(m_id++, 8 + panel_w / 4 * 3 + 4, m_element_y, panel_w / 4, 20, "0", this));

    m_w->set_flags(TEXTBOX_NUMERIC); m_w->set_cutoff(20);
    m_h->set_flags(TEXTBOX_NUMERIC); m_h->set_cutoff(20);
    m_u->set_flags(TEXTBOX_NUMERIC); m_u->set_cutoff(20);
    m_v->set_flags(TEXTBOX_NUMERIC); m_v->set_cutoff(20);

    m_element_y += 40;
}

void DialogNewElement::add_keycode_elements(void)
{
    uint16_t panel_w = 254;

    if (m_element_y == 0)
        m_element_y = 30;
    add(new Label(m_id++, 8, m_element_y, LANG_LABEL_KEY_CODE, this));
    m_element_y += 25;
    add(m_keycode = new Textbox(m_id++, 8, m_element_y, panel_w, 20, "0", this));
    m_keycode->set_cutoff(10);
    m_keycode->set_flags(TEXTBOX_HEX | TEXTBOX_KEYBIND);
    m_element_y += 25;
    add(m_read_keybind = new Checkbox(m_id++, 8, m_element_y, LANG_CHECKBOX_RECORD_KEYBIND, this, true));
    m_element_y += 40;
}

void DialogNewElement::add_element_id(void)
{
    uint16_t panel_w = 254;

    if (m_element_y == 0)
        m_element_y = 30;
    add(new Label(m_id++, 9, m_element_y, LANG_LABEL_ELEMENT_ID, this));
    m_element_y += 25;
    add(m_element_id = new Textbox(m_id++, 8, m_element_y, panel_w, 20, ELEMENT_UNNAMED, this));
    m_element_id->set_flags(TEXTBOX_ALPHA_NUMERIC);
    m_element_y += 40;
}

void DialogNewElement::add_z_level(void)
{
    uint16_t panel_w = 254;

    if (m_element_y == 0)
        m_element_y = 30;
    add(new Label(m_id++, 9, m_element_y, LANG_LABEL_Z_LEVEL, this));
    m_element_y += 25;
    add(m_z_level = new Textbox(m_id++, 8, m_element_y, panel_w, 20, "0", this));
    m_z_level->set_flags(TEXTBOX_NUMERIC);
    m_element_y += 40;
}
