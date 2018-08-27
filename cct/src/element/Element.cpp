/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "Element.hpp"
#include "ElementTexture.hpp"
#include "../util/SDL_Helper.hpp"
#include "../util/Texture.hpp"
#include "../util/CoordinateSystem.hpp"
#include "../../../ccl/ccl.hpp"

Element * Element::read_from_file(ccl_config * file, std::string id, ElementType t, SDL_Point * default_dim)
{
	switch (t)
	{
	case TEXTURE:
		return ElementTexture::read_from_file(file, id, default_dim);
	case BUTTON:
		break;
	case MOUSE_SCROLLWHEEL:
		break;
	case MOUSE_MOVEMENT:
		break;
	case ANALOG_STICK:
		break;
	case TRIGGER:
		break;
	case TEXT:
		break;
	case DPAD_STICK:
		break;
	default:
		return nullptr;
	}
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
			return true;
	}
	return false;
}

Element::Element(ElementType t, std::string id, SDL_Point pos, uint8_t z)
{
		m_type = t;
		m_position.x = pos.x;
		m_position.y = pos.y;
		m_id = id;
		m_z_level = z;
}

inline SDL_Rect Element::read_mapping(ccl_config * file, std::string id, SDL_Point * default_dim)
{
	SDL_Rect result;
	result.x = file->get_int(id + CFG_U);
	result.y = file->get_int(id + CFG_V);

	if ((result.w = file->get_int(id + CFG_WIDTH)) == 0)
		result.w = default_dim->x;

	if ((result.h = file->get_int(id + CFG_HEIGHT)) == 0)
		result.w = default_dim->y;
	return result;
}

inline SDL_Point Element::read_position(ccl_config * file, std::string id)
{
	return { file->get_int(id + CFG_X_POS), file->get_int(id + CFG_Y_POS) };
}

inline uint8_t Element::read_layer(ccl_config * file, std::string id)
{
	return file->get_int(id + CFG_Z_LEVEL);
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

SDL_Rect * Element::get_dim(void)
{
	return &m_dimensions;
}

void Element::set_pos(int x, int y)
{
	m_position.x = x;
	m_position.y = y;
}

void Element::set_id(std::string id)
{
	m_id = id;
}

void Element::set_z_level(uint8_t z)
{
	m_z_level = z;
}

uint8_t Element::get_z_level(void)
{
	return m_z_level;
}

std::string * Element::get_id(void)
{
	return &m_id;
}

int Element::get_x()
{
	return m_position.x;
}

int Element::get_y()
{
	return m_position.y;
}

int Element::get_w()
{
	return m_dimensions.w;
}

int Element::get_h()
{
	return m_dimensions.h;
}

ElementType Element::get_type(void)
{
	return m_type;
}
