/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "ElementButton.hpp"
#include <utility>
#include "../util/Notifier.hpp"
#include "../util/SDL_Helper.hpp"
#include "../util/Texture.hpp"
#include "../util/CoordinateSystem.hpp"
#include "../dialog/DialogNewElement.hpp"
#include "../dialog/DialogElementSettings.hpp"
#include "../../../ccl/ccl.hpp"

ElementButton::ElementButton(std::string id, const SDL_Point pos, const SDL_Rect mapping, const uint16_t vc, const uint8_t z)
    : ElementTexture(BUTTON, std::move(id), pos, mapping, z)
{
    m_keycode = vc;
    m_pressed_mapping = m_mapping;
    m_pressed_mapping.y += m_mapping.h + CFG_INNER_BORDER;
}

ElementError ElementButton::is_valid(Notifier* n, SDL_Helper* h)
{
    auto error = ElementTexture::is_valid(n, h);
    if (error == VALID && m_keycode == 0x0)
    {
        n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_KEYCODE_INVALID));
        error = KEYCODE_INVALID;
    }
    return error;
}

void ElementButton::draw(Texture* atlas, CoordinateSystem* cs, const bool selected, const bool alpha)
{
    get_abs_dim(cs);
    if (m_pressed)
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_pressed_mapping, alpha ? 60 : 255);
    else
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping, alpha ? 60 : 255);

    if (selected)
        cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->palette()->red());
}

void ElementButton::write_to_file(ccl_config* cfg, SDL_Point* default_dim)
{
    ElementTexture::write_to_file(cfg, default_dim);
    const auto comment = "Key code of " + m_id;
    cfg->add_int(m_id + CFG_KEY_CODE, comment, m_keycode, true);
}

void ElementButton::update_settings(DialogNewElement* dialog)
{
    ElementTexture::update_settings(dialog);
    m_pressed_mapping = m_mapping;
    m_pressed_mapping.y += m_mapping.h + CFG_INNER_BORDER;
    m_keycode = dialog->get_vc();
}

void ElementButton::update_settings(DialogElementSettings* dialog)
{
    ElementTexture::update_settings(dialog);
    m_pressed_mapping = m_mapping;
    m_pressed_mapping.y += m_mapping.h + CFG_INNER_BORDER;
    m_keycode = dialog->get_vc();
}

void ElementButton::handle_event(SDL_Event* event, SDL_Helper* helper)
{
    uint32_t button = SDLK_UNKNOWN;
    auto pressed = false;

    switch(event->type)
    /* Fallthrough intended */
    {
    case SDL_KEYDOWN:
        pressed = true;
    case SDL_KEYUP:
        button = event->key.keysym.sym;
        break;
    case SDL_CONTROLLERBUTTONDOWN:
        pressed = true;
    case SDL_CONTROLLERBUTTONUP:
        button = TO_PAD_MASK(event->cbutton.button);
        break;
    case SDL_MOUSEBUTTONDOWN:
        pressed = true;
    case SDL_MOUSEBUTTONUP:
        button = TO_MOUSE_MASK(event->button.button);
        break;
    default: ;
    }

    if (SDL_Helper::sdl_key_to_vc(button) == m_keycode)
        m_pressed = pressed;
}

ElementButton* ElementButton::read_from_file(ccl_config* file, const std::string& id, SDL_Point* default_dim)
{
    return new ElementButton(id, read_position(file, id),
                             read_mapping(file, id, default_dim), file->get_int(id + CFG_KEY_CODE),
                             read_layer(file, id));
}
