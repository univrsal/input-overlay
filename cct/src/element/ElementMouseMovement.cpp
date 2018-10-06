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

ElementMouseMovement::ElementMouseMovement(const std::string& id, const SDL_Point pos,
                                           const SDL_Rect mapping, mouse_movement_type type, const uint16_t radius, const uint8_t z)
    : ElementTexture(MOUSE_MOVEMENT, id, pos, mapping, z), m_type()
{
    m_radius = radius;
}

ElementError ElementMouseMovement::is_valid(Notifier* n, SDL_Helper* h)
{
    auto error = ElementTexture::is_valid(n, h);
    if (error == VALID && m_radius == 0)
    {
        n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_RADIUS_INVALID));
        error = MOUSE_RADIUS;
    }
    return error;
}

void ElementMouseMovement::write_to_file(ccl_config* cfg, SDL_Point* default_dim)
{
    ElementTexture::write_to_file(cfg, default_dim);
    auto comment = "Movement type of " + m_id;
    cfg->add_int(m_id + CFG_MOUSE_TYPE, comment, m_type, true);
    comment = "Movement radius of " + m_id;
    cfg->add_int(m_id + CFG_MOUSE_RADIUS, comment, m_radius, true);
}

void ElementMouseMovement::update_settings(DialogNewElement* dialog)
{
    ElementTexture::update_settings(dialog);
    m_radius = dialog->get_radius();
    m_type = dialog->get_mouse_type();
}

mouse_movement_type ElementMouseMovement::get_mouse_type() const
{
    return m_type;
}

uint16_t ElementMouseMovement::get_radius() const
{
    return m_radius;
}

ElementMouseMovement* ElementMouseMovement::
read_from_file(ccl_config* file, const std::string& id, SDL_Point* default_dim)
{
    mouse_movement_type mmt = DOT;
    if (file->get_int(id + CFG_MOUSE_TYPE) != 0)
        mmt = ARROW;

    return new ElementMouseMovement(id, read_position(file, id),
                                    read_mapping(file, id, default_dim), mmt,
                                    file->get_int(id + CFG_MOUSE_RADIUS), read_layer(file, id));
}
