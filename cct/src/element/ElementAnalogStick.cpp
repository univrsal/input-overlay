#include "ElementAnalogStick.hpp"
#include "../util/SDL_Helper.hpp"
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
    Element::get_abs_dim(cs);

    m_dimensions_scaled.x += m_x_axis * m_radius;
    m_dimensions_scaled.y += m_y_axis * m_radius;
    return &m_dimensions_scaled;
}

void ElementAnalogStick::draw(Texture * atlas, CoordinateSystem * cs, bool selected, bool alpha)
{
        ElementTexture::draw(atlas, cs, selected, alpha);
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
}

void ElementAnalogStick::update_settings(DialogElementSettings * dialog)
{
    ElementTexture::update_settings(dialog);
}

void ElementAnalogStick::handle_event(SDL_Event * event, SDL_Helper * helper)
{
    if (event->type == SDL_JOYAXISMOTION && SDL_abs(event->caxis.value) >= STICK_DEAD_ZONE)
    {
        if (m_stick == STICK_LEFT)
        {
            if (event->caxis.type == SDL_CONTROLLER_AXIS_LEFTY)
                m_x_axis = ((float)event->caxis.value) / STICK_MAX_AMPLITUDE;
            else if (event->caxis.type == SDL_CONTROLLER_AXIS_LEFTY)
                m_y_axis = ((float)event->caxis.value) / STICK_MAX_AMPLITUDE;
        }
        else if (m_stick == STICK_RIGHT)
        {
            if (event->caxis.type == SDL_CONTROLLER_AXIS_RIGHTY)
                m_x_axis = ((float)event->caxis.value) / STICK_MAX_AMPLITUDE;
            else if (event->caxis.type == SDL_CONTROLLER_AXIS_RIGHTY)
                m_y_axis = ((float)event->caxis.value) / STICK_MAX_AMPLITUDE;
        }
    }

#ifdef _DEBUG
    printf("X_AXIS : %.2f, Y_AXIS: %.2f\n", m_x_axis, m_y_axis);
#endif
}

ElementAnalogStick * ElementAnalogStick::read_from_file(ccl_config * file, std::string id, SDL_Point * default_dim)
{
    AnalogStick s;
    switch (file->get_int(id + CFG_STICK_SIDE))
    {
    case STICK_RIGHT:
        s = STICK_RIGHT;
        break;
    default:
        s = STICK_LEFT;
    }
    return new ElementAnalogStick(id, Element::read_position(file, id),
        Element::read_mapping(file, id, default_dim), s,
        file->get_int(id + CFG_STICK_RADIUS), Element::read_layer(file, id));
}
