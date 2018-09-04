/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "ElementTexture.hpp"
#include "../dialog/DialogNewElement.hpp"
#include "../dialog/DialogElementSettings.hpp"
#include "../util/CoordinateSystem.hpp"
#include "../util/Texture.hpp"
#include "../util/Notifier.hpp"
#include "../../../ccl/ccl.hpp"

ElementTexture::ElementTexture(std::string id, SDL_Point pos, SDL_Rect mapping, uint8_t z)
    : Element(ElementType::TEXTURE, id, pos, z)
{
    m_mapping = mapping;
}

ElementTexture::ElementTexture(ElementType t, std::string id, SDL_Point pos, SDL_Rect mapping, uint8_t z)
    : Element(t, id, pos, z)
{
    m_mapping = mapping;
}

ElementError ElementTexture::is_valid(Notifier * n, SDL_Helper * h)
{
    ElementError result = Element::is_valid(n, h);
    if (result == ElementError::VALID && SDL_RectEmpty(&m_mapping))
    {
        n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_SELECTION_EMTPY));
        result = ElementError::MAPPING_EMPTY;
    }
    return result;
}

void ElementTexture::draw(Texture * atlas, CoordinateSystem * cs, bool selected, bool alpha)
{
    get_abs_dim(cs);
    atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping, alpha ? 60 : 255);

    if (selected)
    {
        cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->palette()->red());
    }
}

void ElementTexture::write_to_file(ccl_config * cfg, SDL_Point * default_dim)
{
    Element::write_to_file(cfg, default_dim);
    std::string comment = "Texture position of " + m_id;
    cfg->add_int(m_id + CFG_U, "", m_mapping.x, true);
    cfg->add_int(m_id + CFG_V, comment, m_mapping.y, true);

    comment = "Width and height of " + m_id;

    if (m_mapping.w != default_dim->x)
        cfg->add_int(m_id + CFG_WIDTH, "", m_mapping.w, true);

    if (m_mapping.h != default_dim->y)
        cfg->add_int(m_id + CFG_HEIGHT, comment, m_mapping.h, true);
}

void ElementTexture::update_settings(DialogNewElement * dialog)
{
    Element::update_settings(dialog);
    set_mapping(dialog->get_selection());
}

void ElementTexture::update_settings(DialogElementSettings * dialog)
{
    Element::update_settings(dialog);
    set_mapping(dialog->get_mapping());
}

ElementTexture * ElementTexture::read_from_file(ccl_config * file, std::string id, SDL_Point * default_dim)
{
    return new ElementTexture(id, Element::read_position(file, id),
        Element::read_mapping(file, id, default_dim), Element::read_layer(file, id));
}
