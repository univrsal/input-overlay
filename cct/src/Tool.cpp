/**
 * Created by univrsal on 23.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "Tool.hpp"
#include "Config.hpp"
#include "util/Texture.hpp"
#include "util/SDL_Helper.hpp"
#include "util/Notifier.hpp"
#include "dialog/DialogSetup.hpp"
#include "dialog/DialogElementSettings.hpp"
#include "dialog/DialogHelp.hpp"
#include "dialog/DialogNewElement.hpp"
#include "dialog/DialogElementType.hpp"
#include "element/ElementTexture.hpp"

Tool::Tool(SDL_Helper * helper)
{
    m_helper = helper;
    m_state = IN_SETUP;
}

Tool::~Tool()
{
    m_helper = nullptr;

    if (m_config)
        delete m_config;
    close_toplevel();
    m_config = nullptr;

    if (m_notify)
    {
        delete m_notify;
        m_notify = nullptr;
    }
}

void Tool::program_loop()
{
    m_notify = new Notifier(m_helper);

    m_toplevel = new DialogSetup(m_helper, m_notify, this);
    m_toplevel->init();
    m_helper->set_runflag(&m_run_flag);
    while (m_run_flag)
    {
        handle_input();
        m_helper->clear();

        // Drawing
        switch (m_state)
        {
        case IN_SETUP:
            m_toplevel->draw_background();
            m_toplevel->draw_foreground();
            break;
        case IN_BUILD:
            m_config->draw_elements();
            m_element_settings->draw_background();
            m_element_settings->draw_foreground();
            break;
        case IN_ELEMENT_TYPE:
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
    DialogSetup * s = nullptr;
    Element * e = nullptr;

    switch (type)
    {
    case TOOL_ACTION_SETUP_EXIT:
        s = reinterpret_cast<DialogSetup*>(m_toplevel);
        m_element_settings = new DialogElementSettings(m_helper, this);
        m_config = new Config(s->get_texture_path(),
            s->get_config_path(), s->get_default_dim(), s->get_rulers(), m_helper, m_element_settings);

        if (s->should_load_cfg())
        {
            m_config->read_config(m_notify);
        }
        m_element_settings->init();

        m_queue_close = true;
        m_state = IN_BUILD;
        break;
    case TOOL_ACTION_MOD_ELEMENT_APPLY:
        if (m_config->selected())
        {
            d = reinterpret_cast<DialogNewElement*>(m_toplevel);
            m_config->selected()->update_settings(d);
            m_element_settings->select_element(m_config->selected()); /* Refresh Dialog*/
        }
        m_queue_close = true;
        m_state = IN_BUILD;
        break;
    case TOOL_ACTION_NEW_ELEMENT_ADD:
        d = reinterpret_cast<DialogNewElement*>(m_toplevel);

        switch (d->get_type())
        {
        case BUTTON:
            
            break;
        case TEXTURE:
            e = new ElementTexture(*d->get_id(), SDL_Point{ 0, 0 },
                d->get_selection(), d->get_z_level());
            break;
        case TRIGGER:
            break;
        case TEXT:
            break;
        case MOUSE_SCROLLWHEEL:
            break;
        case DPAD_STICK:
            break;
        case MOUSE_MOVEMENT:
            break;
        case ANALOG_STICK:
            break;
        }

        if (e)
            add_element(e);
        m_queue_close = true;
        m_state = IN_BUILD;
        break;
    case TOOL_ACTION_SAVE_CONFIG:
        m_config->write_config(m_notify);
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

void Tool::queue_dialog_open(DialogID id)
{
    m_queued_dialog = id;
}

void Tool::queue_dialog_close(void)
{
    m_queue_close = true;
    m_state = IN_BUILD;
}

void Tool::add_element(Element * e)
{
    bool can_add = true;
    for (auto const &element : m_config->m_elements)
    {
        if (element->get_id()->compare(e->get_id()->c_str()) == 0)
        {
            can_add = false;
            m_notify->add_msg(MESSAGE_ERROR, m_helper->loc(LANG_ERROR_ID_NOT_UNIQUE));
            break;
        }
    }

    can_add = e->is_valid(m_notify, m_helper);

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
        m_helper->handle_events(&m_event);
        if (m_event.type == SDL_QUIT)
        {
            m_run_flag = false;
        }
        else if (m_helper->is_ctrl_down() && m_event.type == SDL_KEYDOWN)
        {
            if (m_event.key.keysym.sym == SDLK_s) // CTRL + S
            {
                action_performed(TOOL_ACTION_SAVE_CONFIG);
            }
        }
        else if (m_event.type == SDL_JOYDEVICEADDED)
        {
            if (m_event.cdevice.which >= 0 && m_helper->handle_controller_connect(m_event.cdevice.which))
                m_notify->add_msg(MESSAGE_INFO, m_helper->loc(LANG_MSG_GAMEPAD_CONNECTED));
        }
        else if (m_event.type == SDL_JOYDEVICEREMOVED)
        {
            if (m_event.cdevice.which >= 0 && m_helper->handle_controller_disconnect(m_event.cdevice.which))
                m_notify->add_msg(MESSAGE_INFO, m_helper->loc(LANG_MSG_GAMEPAD_DISCONNECTED));
        }
        else if (m_event.type == SDL_WINDOWEVENT)
        {
            if (m_event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                m_notify->resize();
            }
        }

        switch (m_state)
        {
        case IN_ELEMENT_TYPE:
        case IN_NEW_ELEMENT:
        case IN_HELP:
            if (m_config && m_event.type == SDL_WINDOWEVENT
                && m_event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                m_config->handle_events(&m_event); /* Still need it to resize*/
            }
            /* Fall through intended */
        case IN_SETUP:
            if (m_toplevel)
                m_toplevel->handle_events(&m_event);
            break;
        case IN_BUILD:
            if (m_element_settings && !m_element_settings->handle_events(&m_event))
            {
                /* Focused dialog didn't handle the event */
                if (m_config)
                    m_config->handle_events(&m_event);
            }
            break;
        }
    }

    if (m_queue_close)
    {
        m_queue_close = false;
        close_toplevel();
    }

    if (m_queued_dialog != DialogID::NONE)
    {
        DialogNewElement * d = nullptr;
        close_toplevel();
        switch (m_queued_dialog)
        {
        case HELP:
            m_state = IN_HELP;
            m_toplevel = new DialogHelp(m_helper, this);
            m_toplevel->init();
            break;
        case NEW_ELEMENT:
            m_state = IN_NEW_ELEMENT;
            m_toplevel = new DialogNewElement(m_helper, LANG_DIALOG_NEW_ELEMENT, this, m_new_element_type);
            m_toplevel->init();
            d = reinterpret_cast<DialogNewElement*>(m_toplevel);
            d->set_default_dim(m_config->get_default_dim().x, m_config->get_default_dim().y);
            break;
        case MOD_ELEMENT:
            if (m_config->selected())
            {
                m_state = IN_NEW_ELEMENT;
                m_toplevel = new DialogNewElement(m_helper, LANG_DIALOG_NEW_ELEMENT, this,
                    m_config->selected()->get_type());
                m_toplevel->init();
                d = reinterpret_cast<DialogNewElement*>(m_toplevel);
                d->set_default_dim(m_config->get_default_dim().x, m_config->get_default_dim().y);
                d->load_from_element(m_config->selected());
            }
            break;
        case SELECECT_TYPE:
            m_state = IN_ELEMENT_TYPE;
            m_toplevel = new DialogElementType(m_helper, this);
            m_toplevel->init();
            break;
        }
        m_queued_dialog = DialogID::NONE;
    }
}
