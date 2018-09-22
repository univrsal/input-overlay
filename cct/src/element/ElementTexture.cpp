/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "ElementTexture.hpp"
#include <utility>
#include "../dialog/DialogNewElement.hpp"
#include "../dialog/DialogElementSettings.hpp"
#include "../util/CoordinateSystem.hpp"
#include "../util/Texture.hpp"
#include "../util/Notifier.hpp"
#include "../../../ccl/ccl.hpp"

ElementTexture::ElementTexture(const std::string& id, const SDL_Point pos, const SDL_Rect mapping, const uint8_t z)
    : Element(TEXTURE, id, pos, z)
{
    m_mapping = mapping;
}

ElementTexture::ElementTexture(const element_type t, const std::string& id, const SDL_Point pos, const SDL_Rect mapping, const uint8_t z)
    : Element(t, id, pos, z)
{
    m_mapping = mapping;
}

ElementError ElementTexture::is_valid(Notifier* n, SDL_Helper* h)
{
    auto result = Element::is_valid(n, h);
    if (result == VALID && SDL_RectEmpty(&m_mapping))
    {
        n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_SELECTION_EMTPY));
        result = MAPPING_EMPTY;
    }
    return result;
}

void ElementTexture::draw(Texture* atlas, CoordinateSystem* cs, const bool selected, const bool alpha)
{
    get_abs_dim(cs);
    atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping,
        (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);

    if (selected)
        cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->palette()->red());
}

void ElementTexture::write_to_file(ccl_config* cfg, SDL_Point* default_dim)
{
    Element::write_to_file(cfg, default_dim);
    const auto comment = "Mapping of " + m_id;
    cfg->add_rect(m_id + CFG_MAPPING, comment, m_mapping.x,
        m_mapping.y, m_mapping.w, m_mapping.h);
}

void ElementTexture::update_settings(DialogNewElement* dialog)
{
    Element::update_settings(dialog);
    set_mapping(dialog->get_selection());
}

void ElementTexture::update_settings(DialogElementSettings* dialog)
{
    Element::update_settings(dialog);
    set_mapping(dialog->get_mapping());
}

ElementTexture* ElementTexture::read_from_file(ccl_config* file, const std::string& id, SDL_Point* default_dim)
{
    return new ElementTexture(id, read_position(file, id),
                              read_mapping(file, id, default_dim), read_layer(file, id));
}
