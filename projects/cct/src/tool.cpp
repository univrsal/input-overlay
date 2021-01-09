/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2021 univrsal <uni@vrsal.de>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#include "tool.hpp"
#include "config.hpp"
#include "dialog/dialog_element_settings.hpp"
#include "dialog/dialog_element_type.hpp"
#include "dialog/dialog_help.hpp"
#include "dialog/dialog_new_element.hpp"
#include "dialog/dialog_setup.hpp"
#include "util/notifier.hpp"

#include <fstream>

tool::tool(sdl_helper *helper, const char *texture, const char *config) : m_event()
{
    m_helper = helper;
    m_state = IN_SETUP;

    m_texture_path = texture;
    m_config_path = config;
}

tool::~tool()
{
    close_top_level();
    m_helper = nullptr;

    delete m_config;
    delete m_notify;

    m_notify = nullptr;
    m_config = nullptr;
}

void tool::program_loop()
{
    m_notify = new notifier(m_helper);

    m_toplevel = new dialog_setup(m_helper, m_notify, this);
    m_toplevel->init();
    m_helper->set_run_flag(&m_run_flag);

    while (m_run_flag) {
        m_helper->start_frame();
        {
            m_helper->clear();
            handle_input();

            // Drawing
            switch (m_state) {
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

                if (m_toplevel) {
                    m_toplevel->draw_background();
                    m_toplevel->draw_foreground();
                }
                break;
            }
            m_notify->draw(); /* Notifications have top priority */
            m_helper->repaint();
        }
#ifdef _DEBUG
        SDL_SetWindowTitle(m_helper->window(), m_helper->format("io-cct | %.2f fps", m_helper->util_get_fps()).c_str());
#endif
        m_helper->end_frame();
        m_helper->cap_frame();
    }
}

element *tool::get_selected() const
{
    if (m_config)
        return m_config->selected();
    return nullptr;
}

uint16_t tool::get_selected_id() const
{
    return m_config->selected_id();
}

void tool::action_performed(const uint8_t type)
{
    dialog_new_element *d = nullptr;
    dialog_setup *s = nullptr;
    element *e = nullptr;

    switch (type) {
    case TOOL_ACTION_SETUP_EXIT:
        s = dynamic_cast<dialog_setup *>(m_toplevel);
        m_element_settings = new dialog_element_settings(m_helper, this);
        m_config = new config(s->get_texture_path(), s->get_config_path(), s->get_default_dim(), s->get_rulers(),
                              m_helper, m_element_settings);

        if (s->should_load_cfg())
            m_config->read_config(m_notify);
        m_element_settings->init();

        m_queue_close = true;
        m_state = IN_BUILD;
        break;
    case TOOL_ACTION_MOD_ELEMENT_APPLY:
        if (m_config->selected()) {
            d = dynamic_cast<dialog_new_element *>(m_toplevel);
            m_config->selected()->update_settings(d);
            m_element_settings->select_element(m_config->selected()); /* Refresh Dialog*/
        }
        m_queue_close = true;
        m_state = IN_BUILD;
        break;
    case TOOL_ACTION_NEW_ELEMENT_ADD:
        d = dynamic_cast<dialog_new_element *>(m_toplevel);
        e = element::from_dialog(d);

        if (e)
            add_element(e);
        m_queue_close = true;
        m_state = IN_BUILD;
        break;
    case TOOL_ACTION_SAVE_CONFIG:
        m_config->write_config(m_notify);
    default:;
    }
}

texture *tool::get_atlas() const
{
    return m_config->get_texture();
}

void tool::delete_element(const uint16_t id) const
{
    m_config->queue_delete(id);
}

void tool::queue_dialog_open(const dialog_id id)
{
    m_queued_dialog = id;
}

void tool::queue_dialog_close()
{
    m_queue_close = true;
    m_state = IN_BUILD;
}

bool tool::element_id_unique(const char *id) const
{
    for (auto const &element : m_config->m_elements) {
        if (*element->get_id() == id) {
            return false;
        }
    }
    return true;
}

element_error tool::verify_element(dialog_new_element *d, const bool modify_mode) const
{
    /*
        Only check uniqueness if the element is newly added or
        it's id was modified
    */
    if (!modify_mode) {
        if (!element_id_unique(d->get_id()->c_str())) {
            m_notify->add_msg(MESSAGE_ERROR, m_helper->loc(LANG_ERROR_ID_NOT_UNIQUE));
            return EE_ID_NOT_UNIQUE;
        }
    }

    auto e = element::from_dialog(d);
    const auto error = e->is_valid(m_notify, m_helper);
    delete e;
    return error;
}

void tool::add_element(element *e) const
{
    /* Sanitizing is done in verify_element */
    if (e)
        m_config->m_elements.emplace_back(e);
}

void tool::close_top_level()
{
    if (m_toplevel) {
        delete m_toplevel;
        m_toplevel = nullptr;
    }
}

void tool::handle_input()
{
    while (SDL_PollEvent(&m_event) != 0) {
        m_helper->handle_events(&m_event);

        if (m_event.type == SDL_QUIT) {
            m_run_flag = false;
        } else if (m_helper->is_ctrl_down() && m_event.type == SDL_KEYDOWN) {
            if (m_event.key.keysym.sym == SDLK_s) // CTRL + S
                action_performed(TOOL_ACTION_SAVE_CONFIG);
        } else if (m_event.type == SDL_CONTROLLERDEVICEADDED) {
            if (m_helper->handle_controller_connect(m_event.cdevice.which))
                m_notify->add_msg(MESSAGE_INFO, m_helper->loc(LANG_MSG_GAMEPAD_CONNECTED));
        } else if (m_event.type == SDL_CONTROLLERDEVICEREMOVED) {
            if (m_helper->handle_controller_disconnect(m_event.cdevice.which))
                m_notify->add_msg(MESSAGE_INFO, m_helper->loc(LANG_MSG_GAMEPAD_DISCONNECTED));
        } else if (m_event.type == SDL_WINDOWEVENT) {
            if (m_event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                m_notify->resize();
        }

        switch (m_state) {
        case IN_ELEMENT_TYPE:
        case IN_NEW_ELEMENT:
        case IN_HELP:
            if (m_config && m_event.type == SDL_WINDOWEVENT && m_event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                m_config->handle_events(&m_event); /* Still need it to resize*/
            }
            /* Fall through intended */
        case IN_SETUP:
            if (m_toplevel)
                m_toplevel->handle_events(&m_event);
            break;
        case IN_BUILD:
            if (m_element_settings && !m_element_settings->handle_events(&m_event)) {
                /* Focused dialog didn't handle the event */
                if (m_config)
                    m_config->handle_events(&m_event);
            }
            break;
        }
    }

    if (m_queue_close) {
        m_queue_close = false;
        close_top_level();
    }

    if (m_queued_dialog != NONE) {
        dialog_new_element *d = nullptr;
        close_top_level();
        switch (m_queued_dialog) {
        case HELP:
            m_state = IN_HELP;
            m_toplevel = new DialogHelp(m_helper, this);
            m_toplevel->init();
            break;
        case NEW_ELEMENT:
            m_state = IN_NEW_ELEMENT;
            m_toplevel = new dialog_new_element(m_helper, LANG_DIALOG_NEW_ELEMENT, this, m_new_element_type);
            m_toplevel->init();
            d = dynamic_cast<dialog_new_element *>(m_toplevel);
            d->set_default_dim(m_config->get_default_dim().x, m_config->get_default_dim().y);
            break;
        case MOD_ELEMENT:
            if (m_config->selected()) {
                m_state = IN_NEW_ELEMENT;
                m_toplevel =
                    new dialog_new_element(m_helper, LANG_DIALOG_NEW_ELEMENT, this, m_config->selected()->get_type());
                m_toplevel->init();
                d = dynamic_cast<dialog_new_element *>(m_toplevel);
                d->set_default_dim(m_config->get_default_dim().x, m_config->get_default_dim().y);
                d->load_from_element(m_config->selected());
            }
            break;
        case SELECECT_TYPE:
            m_state = IN_ELEMENT_TYPE;
            m_toplevel = new DialogElementType(m_helper, this);
            m_toplevel->init();
            break;
        default:;
        }
        m_queued_dialog = NONE;
    }
}
