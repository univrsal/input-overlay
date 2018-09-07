/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "Element.hpp"
#include "ElementTexture.hpp"
#include "ElementButton.hpp"
#include "ElementAnalogStick.hpp"
#include "ElementScrollWheel.hpp"
#include "ElementMouseMovement.hpp"
#include "ElementTrigger.hpp"
#include "ElementGamepadID.hpp"
#include "../dialog/DialogNewElement.hpp"
#include "../dialog/DialogElementSettings.hpp"
#include "../util/SDL_Helper.hpp"
#include "../util/Texture.hpp"
#include "../util/CoordinateSystem.hpp"
#include "../util/Notifier.hpp"
#include "../util/SDL_Helper.hpp"
#include "../../../ccl/ccl.hpp"

Element * Element::read_from_file(ccl_config * file, std::string id, ElementType t, SDL_Point * default_dim)
{
	switch (t)
	{
	case TEXTURE:
		return ElementTexture::read_from_file(file, id, default_dim);
	case BUTTON:
        return ElementButton::read_from_file(file, id, default_dim);
	case MOUSE_SCROLLWHEEL:
        return ElementScrollWheel::read_from_file(file, id, default_dim);
	case MOUSE_MOVEMENT:
        return ElementMouseMovement::read_from_file(file, id, default_dim);
	case ANALOG_STICK:
        return ElementAnalogStick::read_from_file(file, id, default_dim);
	case TRIGGER:
        return ElementTrigger::read_from_file(file, id, default_dim);
	case TEXT:
		break;
	case DPAD_STICK:
		break;
    case GAMEPAD_ID:
        return ElementGamepadID::read_from_file(file, id, default_dim);
        break;
	}
	return nullptr;
}

 Element * Element::from_dialog(DialogNewElement * dialog)
 {
     Element * e;
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
     case TEXT:
         break;
     case MOUSE_SCROLLWHEEL:
         e = new ElementScrollWheel();
         break;
     case DPAD_STICK:
         break;
     case MOUSE_MOVEMENT:
         e = new ElementMouseMovement();
         break;
     case ANALOG_STICK:
         e = new ElementAnalogStick();
         break;
     case GAMEPAD_ID:
         e = new ElementGamepadID();
         break;
     default:
         e = nullptr;
     }

     if (e)
     {
         e->m_type = dialog->get_type();
         e->update_settings(dialog);
     }
     return e;
 }

bool Element::valid_type(int t)
{
	switch (t)
	{
		case ElementType::ANALOG_STICK:
		case ElementType::BUTTON:
		case ElementType::DPAD_STICK:
		case ElementType::MOUSE_MOVEMENT:
		case ElementType::MOUSE_SCROLLWHEEL:
		case ElementType::TEXT:
		case ElementType::TEXTURE:
		case ElementType::TRIGGER:
        case ElementType::GAMEPAD_ID:
			return true;
	}
	return false;
}

Element::Element()
{
    m_id = "";
    m_type = INVALID;
    m_position = { 0, 0 };
    m_mapping = { 0, 0, 0, 0 };
}

Element::Element(ElementType t, std::string id, SDL_Point pos, uint8_t z)
{
		m_type = t;
		m_position.x = pos.x;
		m_position.y = pos.y;
		m_id = id;
		m_z_level = z;
}

void Element::write_to_file(ccl_config * cfg, SDL_Point * default_dim)
{
	/* Write commonly shared values */
	std::string comment = "Z position (Layer) of " + m_id;
	cfg->add_int(m_id + CFG_Z_LEVEL, comment, m_z_level, true);

	comment = "Type id of " + m_id;
	cfg->add_int(m_id + CFG_TYPE, comment, m_type, true);

	comment = "Position of " + m_id;
	cfg->add_int(m_id + CFG_X_POS, "", m_position.x, true);
	cfg->add_int(m_id + CFG_Y_POS, comment, m_position.y, true);
}

SDL_Rect * Element::get_abs_dim(CoordinateSystem * cs)
{
    m_scale = cs->get_scale();
    m_dimensions_scaled = { m_position.x * cs->get_scale() + cs->get_origin_x(),
        m_position.y * cs->get_scale() + cs->get_origin_y(),
        m_mapping.w * cs->get_scale(),
        m_mapping.h * cs->get_scale() };
	return &m_dimensions_scaled;
}

void Element::update_settings(DialogNewElement * dialog)
{
	set_z_level(dialog->get_z_level());
	if (!dialog->get_id()->empty())
		set_id(*dialog->get_id());
}

void Element::update_settings(DialogElementSettings * dialog)
{
	if (!dialog->get_id()->empty())
		set_id(*dialog->get_id());
	else
		dialog->alert_element_id();
	set_z_level(dialog->get_z());
	set_pos(dialog->get_x(), dialog->get_y());
}

ElementError Element::is_valid(Notifier * n, SDL_Helper * h)
{
    ElementError result = ElementError::VALID;

	if (m_id.empty())
	{
		n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_ID_EMPTY));
		result = ElementError::ID_EMPTY;
	}

	if (m_type == ElementType::INVALID)
	{
		n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_TYPE_INVALID));
		result = ElementError::TYPE_INVALID;
	}
	return result;
}

void Element::set_mapping(SDL_Rect r)
{
	m_mapping = r;
	m_scale = 0; /* Forces a rescale at next draw */
}

void Element::set_pos(int x, int y)
{
	m_position.x = x;
	m_position.y = y;
    m_scale = 0; /* Forces a rescale at next draw */
}

SDL_Rect Element::read_mapping(ccl_config * file, std::string id, SDL_Point * default_dim)
{
	SDL_Rect result;
	result.x = file->get_int(id + CFG_U);
	result.y = file->get_int(id + CFG_V);

	if ((result.w = file->get_int(id + CFG_WIDTH)) == 0)
		result.w = default_dim->x;

	if ((result.h = file->get_int(id + CFG_HEIGHT)) == 0)
		result.h = default_dim->y;
	return result;
}

SDL_Point Element::read_position(ccl_config * file, std::string id)
{
	return { file->get_int(id + CFG_X_POS), file->get_int(id + CFG_Y_POS) };
}

uint8_t Element::read_layer(ccl_config * file, std::string id)
{
	return file->get_int(id + CFG_Z_LEVEL);
}

ElementSide Element::read_side(ccl_config * file, std::string id)
{
    ElementSide s = ElementSide::SIDE_LEFT;
    if (file->get_int(id + CFG_SIDE) != 0)
        s = ElementSide::SIDE_RIGHT;
    return s;
}
