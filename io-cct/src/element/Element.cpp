/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "Element.hpp"
#include <utility>
#include "ElementTexture.hpp"
#include "ElementButton.hpp"
#include "ElementAnalogStick.hpp"
#include "ElementScrollWheel.hpp"
#include "ElementMouseMovement.hpp"
#include "ElementTrigger.hpp"
#include "ElementGamepadID.hpp"
#include "ElementDPad.hpp"
#include "../dialog/DialogNewElement.hpp"
#include "../dialog/DialogElementSettings.hpp"
#include "../util/SDL_Helper.hpp"
#include "../util/Texture.hpp"
#include "../util/CoordinateSystem.hpp"
#include "../util/Notifier.hpp"
#include "../util/SDL_Helper.hpp"
#include "../../../ccl/ccl.hpp"

Element* Element::read_from_file(ccl_config* file, const std::string& id, const element_type t, SDL_Point* default_dim)
{
    switch (t)
    {
    case TEXTURE:
        return ElementTexture::read_from_file(file, id, default_dim);
    case BUTTON:
        return ElementButton::read_from_file(file, id, default_dim);
    case MOUSE_SCROLLWHEEL:
        return ElementScrollWheel::read_from_file(file, id, default_dim);
    case MOUSE_STATS:
        return ElementMouseMovement::read_from_file(file, id, default_dim);
    case ANALOG_STICK:
        return ElementAnalogStick::read_from_file(file, id, default_dim);
    case TRIGGER:
        return ElementTrigger::read_from_file(file, id, default_dim);
    case DPAD_STICK:
        return ElementDPad::read_from_file(file, id, default_dim);
    case GAMEPAD_ID:
        return ElementGamepadID::read_from_file(file, id, default_dim);
    default: ;
    }
    return nullptr;
}

Element* Element::from_dialog(DialogNewElement* dialog)
{
    Element* e = nullptr;
    switch (dialog->get_type())
    {
    case BUTTON:
        e = new ElementButton();
        break;
    case TEXTURE:
        e = new ElementTexture();
        break;
    case TRIGGER:
        e = new ElementTrigger();
        break;
    case MOUSE_SCROLLWHEEL:
        e = new ElementScrollWheel();
        break;
    case DPAD_STICK:
        e = new ElementDPad();
        break;
    case MOUSE_STATS:
        e = new ElementMouseMovement();
        break;
    case ANALOG_STICK:
        e = new ElementAnalogStick();
        break;
    case GAMEPAD_ID:
        e = new ElementGamepadID();
        break;     
    default: ;
    }

    if (e)
    {
        e->m_type = dialog->get_type();
        e->update_settings(dialog);
    }
    return e;
}

bool Element::valid_type(const int t)
{
    switch (t)
    {
    case ANALOG_STICK:
    case BUTTON:
    case DPAD_STICK:
    case MOUSE_STATS:
    case MOUSE_SCROLLWHEEL:
    case TEXTURE:
    case TRIGGER:
    case GAMEPAD_ID:
        return true;
    default: ;
    }
    return false;
}

Element::Element()
{
    m_id = "";
    m_type = INVALID;
    m_position = {0, 0};
    m_mapping = {0, 0, 0, 0};
    m_dimensions_scaled = {};
}

Element::Element(const element_type t, std::string id, const SDL_Point pos, const uint8_t z)
{
    m_mapping = {};
    m_dimensions_scaled = {};
    m_type = t;
    m_position = {pos.x, pos.y };
    m_id = std::move(id);
    m_z_level = z;
}

void Element::write_to_file(ccl_config* cfg, SDL_Point* default_dim, uint8_t& layout_flags)
{
    /* Write commonly shared values */
    auto comment = "Z position (Layer) of " + m_id;
    cfg->add_int(m_id + CFG_Z_LEVEL, comment, m_z_level, true);

    comment = "Type id of " + m_id;
    cfg->add_int(m_id + CFG_TYPE, comment, m_type, true);

    comment = "Position of " + m_id;
    cfg->add_point(m_id + CFG_POS, comment, m_position.x, m_position.y, true);
}

SDL_Rect* Element::get_abs_dim(CoordinateSystem* cs)
{
    m_scale = cs->get_scale();
    m_dimensions_scaled = {
        m_position.x * cs->get_scale() + cs->get_origin_x(),
        m_position.y * cs->get_scale() + cs->get_origin_y(),
        m_mapping.w * cs->get_scale(),
        m_mapping.h * cs->get_scale()
    };
    return &m_dimensions_scaled;
}

void Element::update_settings(DialogNewElement* dialog)
{
    set_z_level(dialog->get_z_level());
    if (!dialog->get_id()->empty())
        set_id(*dialog->get_id());
}

void Element::update_settings(DialogElementSettings* dialog)
{
    if (!dialog->get_id()->empty())
        set_id(*dialog->get_id());
    else
        dialog->alert_element_id();
    set_z_level(dialog->get_z());
    set_pos(dialog->get_x(), dialog->get_y());
}

ElementError Element::is_valid(Notifier* n, SDL_Helper* h)
{
    auto result = VALID;

    if (m_id.empty())
    {
        n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_ID_EMPTY));
        result = ID_EMPTY;
    }

    if (m_type == INVALID)
    {
        n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_TYPE_INVALID));
        result = TYPE_INVALID;
    }
    return result;
}

void Element::set_mapping(const SDL_Rect r)
{
    m_mapping = r;
    m_scale = 0; /* Forces a rescale at next draw */
}

void Element::set_pos(const int x, const int y)
{
    m_position.x = x;
    m_position.y = y;
    m_scale = 0; /* Forces a rescale at next draw */
}

SDL_Rect Element::read_mapping(ccl_config* file, const std::string& id, SDL_Point* default_dim)
{
    const auto temp = file->get_rect(id + CFG_MAPPING);
    return SDL_Rect { temp.x, temp.y, temp.w, temp.h };
}

SDL_Point Element::read_position(ccl_config* file, const std::string& id)
{
    const auto pos = file->get_point(id + CFG_POS);
    return { pos.x, pos.y };
}

uint8_t Element::read_layer(ccl_config* file, const std::string& id)
{
    return file->get_int(id + CFG_Z_LEVEL);
}

element_side Element::read_side(ccl_config* file, const std::string& id)
{
    auto s = SIDE_LEFT;
    if (file->get_int(id + CFG_SIDE) != 0)
        s = SIDE_RIGHT;
    return s;
}
