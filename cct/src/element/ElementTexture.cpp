#include "ElementTexture.hpp"
#include "../util/CoordinateSystem.hpp"
#include "../util/Texture.hpp"
#include "../../../ccl/ccl.hpp"

ElementTexture::ElementTexture(std::string id, SDL_Point pos, SDL_Rect mapping, uint8_t z)
	: Element(ElementType::TEXTURE, id, pos, z)
{
	m_dimensions = mapping;
}

void ElementTexture::draw(Texture * atlas, CoordinateSystem * cs, bool selected, bool alpha)
{
	get_abs_dim(cs);
	atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_dimensions, alpha ? 60 : 255);

	if (selected)
	{
		cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->palette()->red());
	}
}

void ElementTexture::write_to_file(ccl_config * cfg, SDL_Point * default_dim)
{
	Element::write_to_file(cfg, default_dim);
	std::string comment = "Texture position of " + m_id;
	cfg->add_int(m_id + CFG_U, "", m_dimensions.x, true);
	cfg->add_int(m_id + CFG_V, comment, m_dimensions.y, true);

	comment = "Width and height of " + m_id;

	if (m_dimensions.w != default_dim->x)
		cfg->add_int(m_id + CFG_WIDTH, "", m_dimensions.w, true);

	if (m_dimensions.h != default_dim->y)
		cfg->add_int(m_id + CFG_HEIGHT, comment, m_dimensions.h, true);
}

SDL_Rect * ElementTexture::get_abs_dim(CoordinateSystem * cs)
{
	if (m_scale != cs->get_scale())
		/* If scale changed, update */
	{
		m_scale = cs->get_scale();
		m_dimensions_scaled = { m_position.x * cs->get_scale() + cs->get_origin_x(),
			m_position.y * cs->get_scale() + cs->get_origin_y(),
			m_dimensions.w * cs->get_scale(),
			m_dimensions.h * cs->get_scale() };
	}
	return &m_dimensions_scaled;
}

ElementTexture * ElementTexture::read_from_file(ccl_config * file, std::string id, SDL_Point * default_dim)
{
	return new ElementTexture(id, read_position(file, id),
		read_mapping(file, id, default_dim), read_layer(file, id));
}
