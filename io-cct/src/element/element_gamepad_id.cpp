/**
 * Created by universal on 06.09.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#include "element_gamepad_id.hpp"
#include <utility>
#include "../dialog/dialog_new_element.hpp"
#include "../dialog/dialog_element_settings.hpp"
#include "../util/coordinate_system.hpp"
#include "../util/texture.hpp"
#include "../util/palette.hpp"
#include "../../../ccl/ccl.hpp"

ElementGamepadID::ElementGamepadID(const std::string &id, const SDL_Point pos, const SDL_Rect mapping, const uint8_t z)
        : element_texture(GAMEPAD_ID, id, pos, mapping, z)
{
    /* NO-OP */
}

void ElementGamepadID::draw(texture* atlas, coordinate_system* cs, const bool selected, const bool alpha)
{
    get_abs_dim(cs);
    auto temp = m_mapping;
    temp.x += (temp.w + CFG_INNER_BORDER) * m_last_gamepad_id;
    atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &temp,
                (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);

    if (m_state) /* Guide button */
    {
        temp = m_mapping;
        temp.x += (temp.w + CFG_INNER_BORDER) * 4;
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &temp,
                    (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);
    }

    if (selected)
        cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->get_palette()->red());
}

void ElementGamepadID::handle_event(SDL_Event* event, sdl_helper* helper)
{
    switch (event->type) {
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            m_last_gamepad_id = event->caxis.which;
            if (event->cbutton.button == SDL_CONTROLLER_BUTTON_GUIDE)
                m_state = event->type == SDL_CONTROLLERBUTTONDOWN ? STATE_PRESSED : STATE_RELEASED;
            break;
        case SDL_CONTROLLERAXISMOTION:
            m_last_gamepad_id = event->cbutton.which;
            break;
        default:;
    }
    m_last_gamepad_id = UTIL_CLAMP(0, m_last_gamepad_id, 4);
}

ElementGamepadID* ElementGamepadID::read_from_file(ccl_config* file, const std::string &id, SDL_Point* default_dim)
{
    return new ElementGamepadID(id, read_position(file, id), read_mapping(file, id, default_dim), read_layer(file, id));
}

void ElementGamepadID::write_to_file(ccl_config* cfg, SDL_Point* default_dim, uint8_t &layout_flags)
{
    element_texture::write_to_file(cfg, default_dim, layout_flags);
    layout_flags |= FLAG_GAMEPAD;
}
