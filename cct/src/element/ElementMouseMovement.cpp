/**
 * Created by universal on 05.09.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "ElementMouseMovement.hpp"
#include <utility>
#include "../util/Notifier.hpp"
#include "../util/SDL_Helper.hpp"
#include "../dialog/DialogNewElement.hpp"
#include "../dialog/DialogElementSettings.hpp"
#include "../../../ccl/ccl.hpp"

ElementMouseMovement::ElementMouseMovement(std::string id, SDL_Point pos,
    SDL_Rect mapping, MouseMovementType type, uint16_t radius, uint8_t z)
    : ElementTexture(ElementType::MOUSE_MOVEMENT, std::move(id), pos, mapping, z)
{
    m_radius = radius;
    m_type;
}

ElementError ElementMouseMovement::is_valid(Notifier * n, SDL_Helper * h)
{
    ElementError error = ElementTexture::is_valid(n, h);
    if (error == ElementError::VALID && m_radius == 0)
    {
        n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_RADIUS_INVALID));
        error = ElementError::MOUSE_RADIUS;
    }
    return error;
}

void ElementMouseMovement::write_to_file(ccl_config * cfg, SDL_Point * default_dim)
{
    ElementTexture::write_to_file(cfg, default_dim);
    std::string comment = "Movement type of " + m_id;
    cfg->add_int(m_id + CFG_MOUSE_TYPE, comment, m_type, true);
    comment = "Movement radius of " + m_id;
    cfg->add_int(m_id + CFG_MOUSE_RADIUS, comment, m_radius, true);
}

void ElementMouseMovement::update_settings(DialogNewElement * dialog)
{
    ElementTexture::update_settings(dialog);
    m_radius = dialog->get_radius();
    m_type = dialog->get_mouse_type();
}

MouseMovementType ElementMouseMovement::get_mouse_type(void)
{
    return m_type;
}

uint16_t ElementMouseMovement::get_radius(void)
{
    return m_radius;
}

ElementMouseMovement * ElementMouseMovement::
    read_from_file(ccl_config * file, const std::string& id, SDL_Point * default_dim)
{
    MouseMovementType mmt = MouseMovementType::DOT;
    if (file->get_int(id + CFG_MOUSE_TYPE) != 0)
        mmt = MouseMovementType::ARROW;

    return new ElementMouseMovement(id, Element::read_position(file, id),
            Element::read_mapping(file, id, default_dim), mmt,
            file->get_int(id + CFG_MOUSE_RADIUS), Element::read_layer(file, id));
}
