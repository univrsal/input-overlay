/**
 * Created by universal on 05.09.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#include "element_mouse_movement.hpp"
#include <utility>
#include "../util/notifier.hpp"
#include "../util/sdl_helper.hpp"
#include "../dialog/dialog_new_element.hpp"
#include "../dialog/dialog_element_settings.hpp"
#include "../../../ccl/ccl.hpp"

ElementMouseMovement::ElementMouseMovement(const std::string &id, const SDL_Point pos, const SDL_Rect mapping,
                                           const mouse_movement_type type, const uint16_t radius, const uint8_t z)
        : element_texture(MOUSE_STATS, id, pos, mapping, z)
{
    m_radius = radius;
    m_type = type;
}

ElementError ElementMouseMovement::is_valid(notifier* n, sdl_helper* h)
{
    auto error = element_texture::is_valid(n, h);
    if (error == VALID && (m_type == DOT && m_radius == 0)) {
        n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_RADIUS_INVALID));
        error = MOUSE_RADIUS;
    }
    return error;
}

void ElementMouseMovement::write_to_file(ccl_config* cfg, SDL_Point* default_dim, uint8_t &layout_flags)
{
    element_texture::write_to_file(cfg, default_dim, layout_flags);
    auto comment = "Movement type of " + m_id;

    cfg->add_int(m_id + CFG_MOUSE_TYPE, comment, m_type, true);
    if (m_type == DOT) {
        comment = "Movement radius of " + m_id;
        cfg->add_int(m_id + CFG_MOUSE_RADIUS, comment, m_radius, true);
    }

    layout_flags |= FLAG_MOUSE;
}

void ElementMouseMovement::update_settings(dialog_new_element* dialog)
{
    element_texture::update_settings(dialog);
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

ElementMouseMovement*
ElementMouseMovement::read_from_file(ccl_config* file, const std::string &id, SDL_Point* default_dim)
{
    auto mmt = DOT;
    if (file->get_int(id + CFG_MOUSE_TYPE) != 0)
        mmt = ARROW;

    return new ElementMouseMovement(id, read_position(file, id), read_mapping(file, id, default_dim), mmt,
                                    file->get_int(id + CFG_MOUSE_RADIUS), read_layer(file, id));
}
