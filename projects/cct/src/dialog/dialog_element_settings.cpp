/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
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

#include "dialog_element_settings.hpp"
#include "../config.hpp"
#include <utility>

void dialog_element_settings::init()
{
    dialog::init();
    int8_t id = 1;

    /* Labels */
    add<label>(8, 35, LANG_LABEL_WIDTH, this);
    add<label>((m_dimensions.w / 2) + 4, 35, LANG_LABEL_HEIGHT, this);
    add<label>(8, 65, LANG_LABEL_X, this);
    add<label>((m_dimensions.w / 2) + 4, 65, LANG_LABEL_Y, this);
    add<label>(8, 95, LANG_LABEL_U, this);
    add<label>((m_dimensions.w / 2) + 4, 95, LANG_LABEL_V, this);
    add<label>(8, 125, LANG_LABEL_ELEMENT_ID, this);
    add<label>(8, 170, LANG_LABEL_KEY_CODE, this);
    add<label>((m_dimensions.w / 2) + 8, 170, LANG_LABEL_Z_LEVEL, this);

    /* Text boxes */
    m_element_width = new textbox(55, 32, (m_dimensions.w / 2) - 63, 20, "0", this);
    m_element_height = new textbox((m_dimensions.w / 2) + 55, 32, (m_dimensions.w / 2) - 63, 20, "0", this);

    m_element_x = new textbox(55, 62, (m_dimensions.w / 2) - 63, 20, "0", this);
    m_element_y = new textbox((m_dimensions.w / 2) + 55, 62, (m_dimensions.w / 2) - 63, 20, "0", this);

    m_element_u = new textbox(55, 92, (m_dimensions.w / 2) - 63, 20, "0", this);
    m_element_v = new textbox((m_dimensions.w / 2) + 55, 92, (m_dimensions.w / 2) - 63, 20, "0", this);

    m_element_width->set_flags(TEXTBOX_NUMERIC);
    m_element_height->set_flags(TEXTBOX_NUMERIC);
    m_element_x->set_flags(TEXTBOX_NUMERIC);
    m_element_y->set_flags(TEXTBOX_NUMERIC);
    m_element_u->set_flags(TEXTBOX_NUMERIC);
    m_element_v->set_flags(TEXTBOX_NUMERIC);

    m_element_width->set_cutoff(20);
    m_element_height->set_cutoff(20);
    m_element_x->set_cutoff(20);
    m_element_y->set_cutoff(20);
    m_element_u->set_cutoff(20);
    m_element_v->set_cutoff(20);

    add(m_element_width);
    add(m_element_height);
    add(m_element_x);
    add(m_element_y);
    add(m_element_u);
    add(m_element_v);

    add(m_element_id = new textbox(8, 145, m_dimensions.w - 16, 20, "", this));
    add(m_element_vc = new textbox(8, 190, m_dimensions.w / 2 - 16, 20, "", this));

    add(m_element_z_level = new textbox(m_dimensions.w / 2 + 8, 190, m_dimensions.w / 2 - 16, 20, "0", this));

    m_element_id->set_flags(TEXTBOX_NO_SPACE | TEXTBOX_ALPHA_NUMERIC);
    m_element_vc->set_flags(TEXTBOX_HEX | TEXTBOX_NO_SPACE);
    m_element_z_level->set_flags(TEXTBOX_NUMERIC);

    /* Controls */
    add<button>(8, m_dimensions.h - 182, m_dimensions.w - 16, LANG_BUTTON_ADD_ELEMENT, this)->set_id(ACTION_NEW_ELEMENT);
    add<button>(8, m_dimensions.h - 154, m_dimensions.w - 16, LANG_BUTTON_DELETE_ELEMENT, this)
        ->set_id(ACTION_DEL_ELEMENT);
    add<button>(8, m_dimensions.h - 126, m_dimensions.w - 16, LANG_BUTTON_MODIFY_ELEMENT, this)
        ->set_id(ACTION_MOD_ELEMENT);
    add<button>(8, m_dimensions.h - 98, m_dimensions.w - 16, LANG_BUTTON_SAVE_CONFIG, this)->set_id(ACTION_SAVE_CONFIG);
    add<button>(8, m_dimensions.h - 70, m_dimensions.w - 16, LANG_BUTTON_HELP, this)->set_id(ACTION_HELP_BUTTON);
    add<button>(8, m_dimensions.h - 32, LANG_BUTTON_OK, this)->set_id(ACTION_OK);

    set_flags(DIALOG_DRAGGABLE | DIALOG_TEXTINPUT);
}

void dialog_element_settings::action_performed(const int8_t action_id)
{
    switch (action_id) {
    case ACTION_OK:
        if (m_tool->get_selected()) {
            auto const new_id = strcmp(m_element_id->c_str(), m_tool->get_selected()->get_id()->c_str()) != 0;
            if (new_id && !m_tool->element_id_unique(m_element_id->c_str())) {
                m_element_id->set_alert(true);
            } else {
                m_tool->get_selected()->update_settings(this);
            }
        }
        break;
    case ACTION_DEL_ELEMENT:
        if (m_tool->get_selected() && m_tool->get_selected_id() >= 0) {
            m_tool->delete_element(m_tool->get_selected_id());
        }
        break;
    case ACTION_MOD_ELEMENT:
        m_tool->queue_dialog_open(MOD_ELEMENT);
        break;
    case ACTION_HELP_BUTTON:
        m_tool->queue_dialog_open(HELP);
        break;
    case ACTION_NEW_ELEMENT:
        m_tool->queue_dialog_open(SELECECT_TYPE);
        break;
    case ACTION_SAVE_CONFIG:
        m_tool->action_performed(TOOL_ACTION_SAVE_CONFIG);
        break;
    default:;
    }
}

void dialog_element_settings::set_wh(const int w, const int h) const
{
    if (w >= 0) {
        m_element_width->set_text(std::to_string(w));
    }

    if (h >= 0) {
        m_element_height->set_text(std::to_string(h));
    }
}

void dialog_element_settings::set_xy(const int x, const int y) const
{
    if (x >= 0) {
        m_element_x->set_text(std::to_string(x));
    }

    if (y >= 0) {
        m_element_y->set_text(std::to_string(y));
    }
}

void dialog_element_settings::set_uv(const int u, const int v) const
{
    if (u >= 0) {
        m_element_u->set_text(std::to_string(u));
    }

    if (v >= 0) {
        m_element_v->set_text(std::to_string(v));
    }
}

void dialog_element_settings::set_id(std::string id) const
{
    m_element_id->set_text(std::move(id));
}

void dialog_element_settings::set_vc(const uint16_t vc) const
{
    m_element_vc->set_hex_int(vc);
}

void dialog_element_settings::set_z_level(const uint8_t z) const
{
    m_element_z_level->set_text(std::to_string(z));
}

void dialog_element_settings::select_element(element *e) const
{
    if (e) {
        set_id(*e->get_id());
        set_uv(e->get_u(), e->get_v());
        set_xy(e->get_x(), e->get_y());
        set_wh(e->get_w(), e->get_h());
        set_vc(e->get_vc());
        set_z_level(e->get_z_level());
    }
}
