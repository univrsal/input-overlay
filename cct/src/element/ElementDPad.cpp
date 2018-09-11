/**
 * Created by universal on 11.09.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "../dialog/DialogNewElement.hpp"
#include "../dialog/DialogElementSettings.hpp"
#include "../util/CoordinateSystem.hpp"
#include "../util/Texture.hpp"
#include "../../../ccl/ccl.hpp"
#include "ElementDPad.hpp"

void ElementDPad::draw(Texture* atlas, CoordinateSystem* cs, const bool selected, const bool alpha)
{
    get_abs_dim(cs);
    if (m_dir == DPAD_CENTER)
    {
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping, alpha ? 60 : 255);
    }
    else
    {
        auto temp = m_mapping;
        temp.x += (CFG_INNER_BORDER + temp.w) * m_dir;
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &temp, alpha ? 60 : 255);
    }

    if (selected)
        cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->palette()->red());
}

void ElementDPad::handle_event(SDL_Event* event, SDL_Helper* helper)
{
    if (event->type == SDL_CONTROLLERBUTTONDOWN)
    {
        bool flag = false;
        switch (event->cbutton.button)
        {
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            switch (m_last_button)
            {
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                m_dir = DPAD_TOP_LEFT;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                m_dir = DPAD_TOP_RIGHT;
                break;
            default:
                m_dir = DPAD_UP;
            }
            flag = true;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            switch (m_last_button)
            {
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                m_dir = DPAD_BOTTOM_LEFT;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                m_dir = DPAD_BOTTOM_RIGHT;
                break;
            default:
                m_dir = DPAD_DOWN;
            }
            flag = true;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            switch (m_last_button)
            {
            case SDL_CONTROLLER_BUTTON_DPAD_UP:
                m_dir = DPAD_TOP_LEFT;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                m_dir = DPAD_BOTTOM_LEFT;
                break;
            default:
                m_dir = DPAD_LEFT;
            }
            flag = true;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            switch (m_last_button)
            {
            case SDL_CONTROLLER_BUTTON_DPAD_UP:
                m_dir = DPAD_TOP_RIGHT;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                m_dir = DPAD_BOTTOM_RIGHT;
                break;
            default:
                m_dir = DPAD_RIGHT;
            }
            flag = true;
            break;
        default: ;
        }
        if (flag)
            m_last_button = event->cbutton.button;
    }
    else if (event->type == SDL_CONTROLLERBUTTONUP)
    {
        if (event->cbutton.button == m_last_button)
        {
            m_last_button = SDL_CONTROLLER_BUTTON_INVALID;
            m_dir = DPAD_CENTER;
        }
        else
        {
            switch (m_dir)
            {
            case DPAD_TOP_LEFT:
                switch (event->cbutton.button)
                {
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                    m_dir = DPAD_UP;
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_UP:
                    m_dir = DPAD_LEFT;
                    break;
                default: ;
                }
                break;
            case DPAD_TOP_RIGHT:
                switch (event->cbutton.button)
                {
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                    m_dir = DPAD_UP;
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_UP:
                    m_dir = DPAD_RIGHT;
                    break;
                default: ;
                }
                break;
            case DPAD_BOTTOM_LEFT:
                switch (event->cbutton.button)
                {
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                    m_dir = DPAD_DOWN;
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                    m_dir = DPAD_LEFT;
                    break;
                default: ;
                }
                break;
            case DPAD_BOTTOM_RIGHT:
                switch (event->cbutton.button)
                {
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                    m_dir = DPAD_DOWN;
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                    m_dir = DPAD_RIGHT;
                    break;
                default: ;
                }
                break;
            default: ;
            }
        }
    }
}

ElementDPad* ElementDPad::read_from_file(ccl_config* file, const std::string& id, SDL_Point* default_dim)
{
    return new ElementDPad(id, read_position(file, id),
                           read_mapping(file, id, default_dim), read_layer(file, id));
}
