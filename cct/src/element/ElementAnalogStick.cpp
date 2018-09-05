/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "ElementAnalogStick.hpp"
#include "../util/SDL_Helper.hpp"
#include "../util/Notifier.hpp"
#include "../util/Texture.hpp"
#include "../util/CoordinateSystem.hpp"
#include "../dialog/DialogNewElement.hpp"
#include "../dialog/DialogElementSettings.hpp"
#include "../../../ccl/ccl.hpp"

ElementAnalogStick::ElementAnalogStick(std::string id, SDL_Point pos, SDL_Rect mapping,
    AnalogStick side, uint8_t radius, uint8_t z)
    : ElementTexture(ElementType::ANALOG_STICK, id, pos, mapping, z)
{
    m_stick = side;
    m_radius = radius;
}

SDL_Rect * ElementAnalogStick::get_abs_dim(CoordinateSystem * cs)
{
    m_static_scaled = * Element::get_abs_dim(cs);

    m_dimensions_scaled.x += m_x_axis * m_radius * cs->get_scale();
    m_dimensions_scaled.y += m_y_axis * m_radius * cs->get_scale();
    return &m_dimensions_scaled;
}

ElementError ElementAnalogStick::is_valid(Notifier * n, SDL_Helper * h)
{
    ElementError error = ElementTexture::is_valid(n, h);

    if (error == ElementError::VALID && m_radius == 0)
    {
        n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_RADIUS_INVALID));
        error = ElementError::STICK_RADIUS;
    }
    return error;
}

void ElementAnalogStick::draw(Texture * atlas, CoordinateSystem * cs, bool selected, bool alpha)
{
    get_abs_dim(cs);
    if (m_pressed)
    {
        SDL_Rect temp = m_mapping;
        temp.y += temp.h + CFG_INNER_BORDER;
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &temp, alpha ? ELEMENT_HIDE_ALPHA : 255);
    }
    else
    {
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping, alpha ? ELEMENT_HIDE_ALPHA : 255);
    }

    if (selected)
    {
        cs->get_helper()->util_draw_rect(&m_static_scaled, cs->get_helper()->palette()->red());
    }

    if (m_movement_reset.started() && m_movement_reset.get_time() >= STICK_RESET)
    {
        m_movement_reset.stop();
        m_x_axis = 0.f, m_y_axis = 0.f;
    }
}

void ElementAnalogStick::write_to_file(ccl_config * cfg, SDL_Point * default_dim)
{
    ElementTexture::write_to_file(cfg, default_dim);
    std::string comment = "Analog stick side of " + m_id;
    cfg->add_int(m_id + CFG_STICK_SIDE, comment, (int) m_stick, true);
}

void ElementAnalogStick::update_settings(DialogNewElement * dialog)
{
        ElementTexture::update_settings(dialog);
        m_radius = dialog->get_radius();
        m_stick = dialog->get_stick();
}

void ElementAnalogStick::update_settings(DialogElementSettings * dialog)
{
    ElementTexture::update_settings(dialog);
}

void ElementAnalogStick::handle_event(SDL_Event * event, SDL_Helper * helper)
{
    if (event->type == SDL_CONTROLLERAXISMOTION && SDL_abs(event->caxis.value) >= STICK_DEAD_ZONE)
    {
        if (m_stick == STICK_LEFT)
        {
            if (event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
            {
                m_x_axis = ((float)event->caxis.value) / STICK_MAX_AMPLITUDE;
                m_movement_reset.start();
            }
            else if (event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
            {
                m_y_axis = ((float)event->caxis.value) / STICK_MAX_AMPLITUDE;
                m_movement_reset.start();
            }
        }
        else
        {
            if (event->caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX)
            {
                m_x_axis = ((float)event->caxis.value) / STICK_MAX_AMPLITUDE;
                m_movement_reset.start();
            }
            else if (event->caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY)
            {
                m_y_axis = ((float)event->caxis.value) / STICK_MAX_AMPLITUDE;
                m_movement_reset.start();
            }
        }
        
#if _DEBUG
        printf("STICK: %i, SDL_EVENT: axis: %i, value: %i\n", m_stick, event->caxis.axis, event->caxis.value);
        printf("X_AXIS : %.2f, Y_AXIS: %.2f\n", m_x_axis, m_y_axis);
#endif
    }
    else if (event->type == SDL_CONTROLLERBUTTONDOWN
        || event->type == SDL_CONTROLLERBUTTONUP)
    {
        if (m_stick == STICK_LEFT)
        {
            if (event->cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSTICK)
                m_pressed = event->cbutton.state == SDL_PRESSED;
        }
        else
        {
            if (event->cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSTICK)
                m_pressed = event->cbutton.state == SDL_PRESSED;
        }
    }
}

ElementAnalogStick * ElementAnalogStick::read_from_file(ccl_config * file, std::string id, SDL_Point * default_dim)
{
    AnalogStick s = AnalogStick::STICK_LEFT;
    if (file->get_int(id + CFG_STICK_SIDE) != 0)
        s = AnalogStick::STICK_RIGHT;

    return new ElementAnalogStick(id, Element::read_position(file, id),
        Element::read_mapping(file, id, default_dim), s,
        file->get_int(id + CFG_STICK_RADIUS), Element::read_layer(file, id));
}
