/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "ElementButton.hpp"
#include "../util/SDL_Helper.hpp"
#include "../util/Texture.hpp"
#include "../util/CoordinateSystem.hpp"
#include "../dialog/DialogNewElement.hpp"
#include "../dialog/DialogElementSettings.hpp"
#include "../../../ccl/ccl.hpp"

ElementButton::ElementButton(std::string id, SDL_Point pos, SDL_Rect mapping, uint16_t vc, uint8_t z)
    : ElementTexture(ElementType::BUTTON, id, pos, mapping, z)
{
    m_keycode = vc;
}

void ElementButton::draw(Texture * atlas, CoordinateSystem * cs, bool selected, bool alpha)
{
    get_abs_dim(cs);
    if (m_pressed)
    {
        SDL_Rect temp = m_mapping;
        temp.y += temp.h + CFG_INNER_BORDER;
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &temp, alpha ? 60 : 255);
    }
    else
    {
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping, alpha ? 60 : 255);
    }

    if (selected)
    {
        cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->palette()->red());
    }
}

void ElementButton::write_to_file(ccl_config * cfg, SDL_Point * default_dim)
{
       ElementTexture::write_to_file(cfg, default_dim);
       std::string comment = "Key code of " + m_id;
       cfg->add_int(m_id + CFG_KEY_CODE, comment, m_keycode, true);
}

void ElementButton::update_settings(DialogNewElement * dialog)
{
    ElementTexture::update_settings(dialog);
    m_keycode = dialog->get_vc();
}

void ElementButton::update_settings(DialogElementSettings * dialog)
{
    ElementTexture::update_settings(dialog);
    m_keycode = dialog->get_vc();
}

void ElementButton::handle_event(SDL_Event * event, SDL_Helper * helper)
{
    if (event->type == SDL_KEYDOWN)
    {
        if (helper->sdl_key_to_vc(event->key.keysym.sym) == m_keycode)
        {
            m_pressed = true;
        }
    }
    else if (event->type == SDL_KEYUP)
    {
        if (helper->sdl_key_to_vc(event->key.keysym.sym) == m_keycode)
        {
            m_pressed = false;
        }
    }
}

ElementButton * ElementButton::read_from_file(ccl_config * file, std::string id, SDL_Point * default_dim)
{
    return new ElementButton(id, Element::element_read_position(file, id),
        Element::element_read_mapping(file, id, default_dim), file->get_int(id + CFG_KEY_CODE),
        Element::element_read_layer(file, id));
}
