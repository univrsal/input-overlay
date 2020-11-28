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

#include "dialog_help.hpp"

void DialogHelp::init()
{
    dialog::init();
    const auto info = add<label>(8, 32, LANG_LABEL_HELP_AND_ABOUT, this);

    add<button>(8, m_dimensions.h - 32, LANG_BUTTON_OK, this)->set_id(ACTION_OK);
    add<button>(116, m_dimensions.h - 32, LANG_BUTTON_OPEN_URL, this);
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
    default:;
    }
}
