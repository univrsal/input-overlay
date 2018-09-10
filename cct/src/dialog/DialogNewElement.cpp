#include "DialogNewElement.hpp"
#include "../Config.hpp"
#include "../element/ElementAnalogStick.hpp"
#include "../element/ElementMouseMovement.hpp"
#include "../element/ElementTrigger.hpp"

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

        m_z_level->set_text(std::to_string(e->get_z_level()));

        m_element_id->set_text(*e->get_id());
        m_initial_name = *e->get_id();

        if (m_keycode)
            m_keycode->set_hex_int(e->get_vc());

        m_selection.x = e->get_u();
        m_selection.y = e->get_v();
        m_selection.w = e->get_w();
        m_selection.h = e->get_h();

        m_modify_mode = true;

        /* Specific handling */
        ElementAnalogStick * stick = nullptr;
        ElementMouseMovement * mouse = nullptr;
        ElementTrigger * trigger = nullptr;

        switch (e->get_type())
        {
        case ANALOG_STICK:
            stick = reinterpret_cast<ElementAnalogStick*>(e);
            m_binary_choice->select_item(stick->get_stick());
            m_radius->set_text(std::to_string(stick->get_radius()));
            break;
        case MOUSE_MOVEMENT:
            mouse = reinterpret_cast<ElementMouseMovement*>(e);
            m_binary_choice->select_item(mouse->get_mouse_type());
            m_radius->set_text(std::to_string(mouse->get_radius()));
        case TRIGGER:
            trigger = reinterpret_cast<ElementTrigger*>(e);
            m_binary_choice->select_item(trigger->get_side());
            m_direction->select_item(trigger->get_direction());
            m_trigger_mode->set_checked(trigger->get_mode());
            break;
        default: ;
        }
    }
}

void DialogNewElement::init()
{
    set_flags(DIALOG_TOP_MOST | DIALOG_FLUID);
    Dialog::init();
    m_element_y = get_top() + 30;

    add_element_id();
    add_z_level();

    if (m_type == ANALOG_STICK)
        add_mouse_or_analog_stick(LANG_LABEL_STICK_SIDE, LANG_LEFT, LANG_RIGHT);
    else if (m_type == BUTTON)
        add_keycode_elements();
    else if (m_type == MOUSE_SCROLLWHEEL)
        add_info(LANG_LABEL_WHEEL_INFO);
    else if (m_type == MOUSE_MOVEMENT)
        add_mouse_or_analog_stick(LANG_LABEL_MOUSE_TYPE,
            LANG_ITEM_MOUSE_TYPE_DOT, LANG_ITEM_MOUSE_TYPE_ARROW);
    else if (m_type == TRIGGER)
        add_trigger();
    else if (m_type == GAMEPAD_ID)
        add_info(LANG_LABEL_GAMEPAD_ID_INFO);

    switch (m_type)
    {
    case ANALOG_STICK:
    case BUTTON:
    case TEXTURE:
    case MOUSE_SCROLLWHEEL:
    case MOUSE_MOVEMENT:
    case TRIGGER:
    case GAMEPAD_ID:
        add(m_selector = new AtlasSelector(m_id++, get_left() + 270,
            get_top() + 30, m_dimensions.w - 278, m_dimensions.h - 38, m_tool->get_atlas(), this));
        m_selector->set_selection(&m_selection);
        add_selection_elements();
        break;
    case INVALID: break;
    case TEXT: break;
    case DPAD_STICK: break;
    default: ;
    }

    add(m_ok = new Button(ACTION_OK, 8, m_dimensions.h - 32, LANG_BUTTON_OK, this));
    add(m_cancel = new Button(ACTION_CANCEL, 124, m_dimensions.h - 32, LANG_BUTTON_CANCEL, this));
}

void DialogNewElement::action_performed(const int8_t action_id)
{
    ElementError error;
    const auto name_unmodified = m_initial_name == m_element_id->c_str();

    switch (action_id)
    {
    case ACTION_OK:
        error = m_tool->verify_element(this, m_modify_mode && name_unmodified);
        if (error == ElementError::VALID)
        {
            if (m_modify_mode)
                m_tool->action_performed(TOOL_ACTION_MOD_ELEMENT_APPLY);
            else
                m_tool->action_performed(TOOL_ACTION_NEW_ELEMENT_ADD);
        }
        else
        {
            handle_error(error);
        }
        break;
    case ACTION_CANCEL:
        m_tool->queue_dialog_close();
        break;
    default:
        if (m_read_keybind)
        {
            if (m_read_keybind->get_state())
                m_keycode->set_flags(TEXTBOX_HEX | TEXTBOX_KEYBIND);
            else
                m_keycode->set_flags(TEXTBOX_HEX);
        }
    }
}

void DialogNewElement::draw_background(void)
{
    Dialog::draw_background();
}

void DialogNewElement::draw_foreground(void)
{
    Dialog::draw_foreground();
}

bool DialogNewElement::handle_events(SDL_Event * event)
{
    auto was_handled = false;
    if (Dialog::handle_events(event))
        was_handled = true;

    switch (m_type)
    {
    case TEXTURE:
    case BUTTON:
    case MOUSE_MOVEMENT:
    case ANALOG_STICK:
    case MOUSE_SCROLLWHEEL:
    case GAMEPAD_ID:
    case TRIGGER:
        if (event->type == SDL_WINDOWEVENT)
        {
            if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                const struct SDL_Rect t = {
                    get_left() + 270,
                    get_top() + 30, m_dimensions.w - 278, m_dimensions.h - 38
                };
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
    default: ;
    }

    return was_handled;
}

ElementType DialogNewElement::get_type(void) const
{
    return m_type;
}

SDL_Rect DialogNewElement::get_selection(void) const
{
    return m_selection;
}

uint16_t DialogNewElement::get_vc(void) const
{
    uint16_t t = 0x0;

    if (m_keycode)
    {
        t = static_cast<uint16_t>(strtoul(m_keycode->get_text()->c_str(),
                nullptr, 16));
    }

    return t;
}

uint8_t DialogNewElement::get_z_level(void) const
{
    return SDL_strtol(m_z_level->get_text()->c_str(), nullptr, 10);
}

ElementSide DialogNewElement::get_side(void) const
{
    if (m_binary_choice)
    {
        switch (m_binary_choice->get_selected())
        {
        case SIDE_RIGHT:
            return SIDE_RIGHT;
        default:
            return SIDE_LEFT;
        }
    }
    return SIDE_LEFT;
}

TriggerDirection DialogNewElement::get_direction(void) const
{
    if (m_direction)
    {
        switch (m_direction->get_selected())
        {
        default:
        case UP:
            return UP;
        case DOWN:
            return DOWN;
        case LEFT:
            return LEFT;
        case RIGHT:
            return RIGHT;
        }
    }
    return UP;
}

MouseMovementType DialogNewElement::get_mouse_type(void) const
{
    if (m_binary_choice)
    {
        switch (m_binary_choice->get_selected())
        {
        case DOT:
            return DOT;
        default:
            return ARROW;
        }
    }
    return DOT;
}

uint8_t DialogNewElement::get_radius(void) const
{
    if (m_radius)
        return SDL_strtol(m_radius->c_str(), nullptr, 10);
    return 0;
}

const std::string * DialogNewElement::get_id(void) const
{
    return m_element_id->get_text();
}

void DialogNewElement::set_default_dim(int w, int h)
{
    m_selection.w = w;
    m_selection.h = h;
    m_selection.x = 1;
    m_selection.y = 1;
    m_w->set_text(std::to_string(w));
    m_h->set_text(std::to_string(h));
    m_u->set_text(std::to_string(1));
    m_v->set_text(std::to_string(1));
}

void DialogNewElement::handle_error(ElementError e) const
{
    switch (e)
    {
    case ID_EMPTY:
    case ID_NOT_UNIQUE:
        m_element_id->set_alert(true);
        break;
    case MAPPING_EMPTY:
        m_u->set_alert(true);
        m_v->set_alert(true);
        m_w->set_alert(true);
        m_h->set_alert(true);
        break;
    case KEYCODE_INVALID:
        m_keycode->set_alert(true);
        break;
    case STICK_RADIUS:
        m_radius->set_alert(true);
        break;
    default: ;
    }
}

void DialogNewElement::add_selection_elements(void)
{

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
    if (m_element_y == 0)
        m_element_y = 30;
    add(new Label(m_id++, 9, m_element_y, LANG_LABEL_Z_LEVEL, this));
    m_element_y += 25;
    add(m_z_level = new Textbox(m_id++, 8, m_element_y, panel_w, 20, "1", this));
    m_z_level->set_flags(TEXTBOX_NUMERIC);
    m_element_y += 40;
}

void DialogNewElement::add_mouse_or_analog_stick(const char * label,
    const char * item_a, const char * item_b)
{
    if (m_element_y == 0)
        m_element_y = 30;
    add(new Label(m_id++, 9, m_element_y, label, this));
    m_element_y += 25;
    add(m_binary_choice = new Combobox(m_id++, 8, m_element_y, panel_w, 20, this));
    m_binary_choice->add_item(item_a);
    m_binary_choice->add_item(item_b);
    m_element_y += 25;
    add(new Label(m_id++, 9, m_element_y, LANG_LABEL_MOVEMENT_RADIUS, this));
    m_element_y += 25;
    add(m_radius = new Textbox(m_id++, 8, m_element_y, panel_w, 20, "0", this));
    m_z_level->set_flags(TEXTBOX_NUMERIC);
    m_radius->set_flags(TEXTBOX_NUMERIC);
    m_element_y += 40;
}

void DialogNewElement::add_info(const char * unlocalized_text)
{
    if (m_element_y == 0)
        m_element_y = 30;
    auto * l = new Label(m_id++, 9, m_element_y, unlocalized_text, this);
    add(l);
    m_element_y += 15 + l->get_height();
}

void DialogNewElement::add_trigger(void)
{

    if (m_element_y == 0)
        m_element_y = 30;
    add(new Label(m_id++, 9, m_element_y, LANG_LABEL_TRIGGER_SIDE, this));
    m_element_y += 25;
    add(m_binary_choice = new Combobox(m_id++, 8, m_element_y, panel_w, 20, this));
    m_binary_choice->add_item(LANG_LEFT);
    m_binary_choice->add_item(LANG_RIGHT);
    m_element_y += 25;

    add(new Label(m_id++, 9, m_element_y, LANG_LABEL_TRIGGER_DIRECTION, this));
    m_element_y += 25;
    add(m_direction = new Combobox(m_id++, 8, m_element_y, panel_w, 20, this));
    m_direction->add_item(LANG_UP);
    m_direction->add_item(LANG_DOWN);
    m_direction->add_item(LANG_LEFT);
    m_direction->add_item(LANG_RIGHT);
    m_element_y += 25;

    add(m_trigger_mode = new Checkbox(m_id++, 8, m_element_y, LANG_CHECKBOX_TRIGGER_BUTON, this));
    m_element_y += 40;
}
