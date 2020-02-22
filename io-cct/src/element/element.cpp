/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <universailp@web.de>.
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

#include "element.hpp"
#include "../../../ccl/ccl.hpp"
#include "../dialog/dialog_element_settings.hpp"
#include "../dialog/dialog_new_element.hpp"
#include "../util/coordinate_system.hpp"
#include "../util/notifier.hpp"
#include "../util/sdl_helper.hpp"
#include "../util/texture.hpp"
#include "element_analog_stick.hpp"
#include "element_button.hpp"
#include "element_dpad.hpp"
#include "element_gamepad_id.hpp"
#include "element_mouse_movement.hpp"
#include "element_scroll_wheel.hpp"
#include "element_texture.hpp"
#include "element_trigger.hpp"
#include <utility>

element* element::read_from_file(ccl_config* file, const std::string& id, const element_type t, SDL_Point* default_dim)
{
    switch (t) {
    case ET_TEXTURE:
        return element_texture::read_from_file(file, id, default_dim);
    case ET_BUTTON:
        return ElementButton::read_from_file(file, id, default_dim);
    case ET_WHEEL:
        return ElementScrollWheel::read_from_file(file, id, default_dim);
    case ET_MOUSE_STATS:
        return ElementMouseMovement::read_from_file(file, id, default_dim);
    case ET_ANALOG_STICK:
        return ElementAnalogStick::read_from_file(file, id, default_dim);
    case ET_TRIGGER:
        return element_trigger::read_from_file(file, id, default_dim);
    case ET_DPAD_STICK:
        return ElementDPad::read_from_file(file, id, default_dim);
    case ET_GAMEPAD_ID:
        return ElementGamepadID::read_from_file(file, id, default_dim);
    default:;
    }
    return nullptr;
}

element* element::from_dialog(dialog_new_element* dialog)
{
    element* e = nullptr;
    switch (dialog->get_type()) {
    case ET_BUTTON:
        e = new ElementButton();
        break;
    case ET_TEXTURE:
        e = new element_texture();
        break;
    case ET_TRIGGER:
        e = new element_trigger();
        break;
    case ET_WHEEL:
        e = new ElementScrollWheel();
        break;
    case ET_DPAD_STICK:
        e = new ElementDPad();
        break;
    case ET_MOUSE_STATS:
        e = new ElementMouseMovement();
        break;
    case ET_ANALOG_STICK:
        e = new ElementAnalogStick();
        break;
    case ET_GAMEPAD_ID:
        e = new ElementGamepadID();
        break;
    default:;
    }

    if (e) {
        e->m_type = dialog->get_type();
        e->update_settings(dialog);
    }
    return e;
}

bool element::valid_type(const int t)
{
    switch (t) {
    case ET_ANALOG_STICK:
    case ET_BUTTON:
    case ET_DPAD_STICK:
    case ET_MOUSE_STATS:
    case ET_WHEEL:
    case ET_TEXTURE:
    case ET_TRIGGER:
    case ET_GAMEPAD_ID:
        return true;
    default:;
    }
    return false;
}

element::element()
{
    m_id = "";
    m_type = ET_INVALID;
    m_position = { 0, 0 };
    m_mapping = { 0, 0, 0, 0 };
    m_dimensions_scaled = {};
}

element::element(const element_type t, std::string id, const SDL_Point pos, const uint8_t z)
{
    m_mapping = {};
    m_dimensions_scaled = {};
    m_type = t;
    m_position = { pos.x, pos.y };
    m_id = std::move(id);
    m_z_level = z;
}

void element::write_to_file(ccl_config* cfg, SDL_Point* default_dim, uint8_t& layout_flags)
{
    /* Write commonly shared values */
    auto comment = "Z position (Layer) of " + m_id;
    cfg->add_int(m_id + CFG_Z_LEVEL, comment, m_z_level, true);

    comment = "Type id of " + m_id;
    cfg->add_int(m_id + CFG_TYPE, comment, m_type, true);

    comment = "Position of " + m_id;
    cfg->add_point(m_id + CFG_POS, comment, m_position.x, m_position.y, true);
}

SDL_Rect* element::get_abs_dim(coordinate_system* cs)
{
    m_scale = cs->get_scale();
    m_dimensions_scaled = { m_position.x * cs->get_scale() + cs->get_origin_x(),
        m_position.y * cs->get_scale() + cs->get_origin_y(), m_mapping.w * cs->get_scale(),
        m_mapping.h * cs->get_scale() };
    return &m_dimensions_scaled;
}

void element::update_settings(dialog_new_element* dialog)
{
    set_z_level(dialog->get_z_level());
    if (!dialog->get_id()->empty())
        set_id(*dialog->get_id());
}

void element::update_settings(dialog_element_settings* dialog)
{
    if (!dialog->get_id()->empty())
        set_id(*dialog->get_id());
    else
        dialog->alert_element_id();
    set_z_level(dialog->get_z());
    set_pos(dialog->get_x(), dialog->get_y());
}

element_error element::is_valid(notifier* n, sdl_helper* h)
{
    auto result = EE_VALID;

    if (m_id.empty()) {
        n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_ID_EMPTY));
        result = EE_ID_EMPTY;
    }

    if (m_type == ET_INVALID) {
        n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_TYPE_INVALID));
        result = EE_TYPE_INVALID;
    }
    return result;
}

void element::set_mapping(const SDL_Rect r)
{
    m_mapping = r;
    m_scale = 0; /* Forces a rescale at next draw */
}

void element::set_pos(const int x, const int y)
{
    m_position.x = x;
    m_position.y = y;
    m_scale = 0; /* Forces a rescale at next draw */
}

SDL_Rect element::read_mapping(ccl_config* file, const std::string& id, SDL_Point* default_dim)
{
    const auto temp = file->get_rect(id + CFG_MAPPING);
    return SDL_Rect { temp.x, temp.y, temp.w, temp.h };
}

SDL_Point element::read_position(ccl_config* file, const std::string& id)
{
    const auto pos = file->get_point(id + CFG_POS);
    return { pos.x, pos.y };
}

uint8_t element::read_layer(ccl_config* file, const std::string& id)
{
    return file->get_int(id + CFG_Z_LEVEL);
}

element_side element::read_side(ccl_config* file, const std::string& id)
{
    auto s = ES_LEFT;
    if (file->get_int(id + CFG_SIDE) != 0)
        s = ES_RIGHT;
    return s;
}
