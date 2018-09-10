/**
 * Created by universal on 06.09.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "ElementGamepadID.hpp"
#include <utility>
#include "../dialog/DialogNewElement.hpp"
#include "../dialog/DialogElementSettings.hpp"
#include "../util/CoordinateSystem.hpp"
#include "../util/Texture.hpp"
#include "../../../ccl/ccl.hpp"

ElementGamepadID::ElementGamepadID(std::string id, const SDL_Point pos, const SDL_Rect mapping, const uint8_t z)
    : ElementTexture(GAMEPAD_ID, std::move(id), pos, mapping, z)
{
    /* NO-OP */
}

void ElementGamepadID::draw(Texture* atlas, CoordinateSystem* cs, const bool selected, const bool alpha)
{
    get_abs_dim(cs);
    auto temp = m_mapping;
    temp.x += (temp.w + CFG_INNER_BORDER) * m_last_gamepad_id;
    atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &temp, alpha ? ELEMENT_HIDE_ALPHA : 255);

    if (selected)
        cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->palette()->red());
}

void ElementGamepadID::handle_event(SDL_Event* event, SDL_Helper* helper)
{
    switch (event->type)
    {
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:
        m_last_gamepad_id = event->caxis.which;
        break;
    case SDL_CONTROLLERAXISMOTION:
        m_last_gamepad_id = event->cbutton.which;
        break;
    default: ;
    }
    m_last_gamepad_id = UTIL_CLAMP(0, m_last_gamepad_id, 4);
}

ElementGamepadID* ElementGamepadID::read_from_file(ccl_config* file, const std::string& id, SDL_Point* default_dim)
{
    return new ElementGamepadID(id, read_position(file, id),
                                read_mapping(file, id, default_dim), read_layer(file, id));
}
