/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "element_analog_stick.hpp"
#include <utility>
#include "../util/sdl_helper.hpp"
#include "../util/notifier.hpp"
#include "../util/texture.hpp"
#include "../util/coordinate_system.hpp"
#include "../util/palette.hpp"
#include "../dialog/dialog_new_element.hpp"
#include "../dialog/dialog_element_settings.hpp"
#include "../../../ccl/ccl.hpp"

ElementAnalogStick::ElementAnalogStick(const std::string& id, const SDL_Point pos, const SDL_Rect mapping,
                                       const element_side side, const uint8_t radius, const uint8_t z) : element_texture(
        ANALOG_STICK, id, pos, mapping, z), m_static_scaled()
{
    m_stick = side;
    m_radius = radius;
}

SDL_Rect* ElementAnalogStick::get_abs_dim(coordinate_system* cs)
{
    m_static_scaled = *element::get_abs_dim(cs);

    m_dimensions_scaled.x += m_x_axis * m_radius * cs->get_scale();
    m_dimensions_scaled.y += m_y_axis * m_radius * cs->get_scale();
    return &m_dimensions_scaled;
}

ElementError ElementAnalogStick::is_valid(notifier* n, sdl_helper* h)
{
    auto error = element_texture::is_valid(n, h);

    if (error == VALID && m_radius == 0) {
        n->add_msg(MESSAGE_ERROR, h->loc(LANG_ERROR_RADIUS_INVALID));
        error = STICK_RADIUS;
    }
    return error;
}

void ElementAnalogStick::draw(texture* atlas, coordinate_system* cs, const bool selected, const bool alpha)
{
    get_abs_dim(cs);
    if (m_pressed) {
        auto temp = m_mapping;
        temp.y += temp.h + CFG_INNER_BORDER;
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &temp,
                    (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);
    } else {
        atlas->draw(cs->get_helper()->renderer(), &m_dimensions_scaled, &m_mapping,
                    (alpha && !selected) ? ELEMENT_HIDE_ALPHA : 255);
    }

    if (selected) {
        cs->get_helper()->util_draw_rect(&m_static_scaled, cs->get_helper()->get_palette()->red());
    }

    if (m_movement_reset.started() && m_movement_reset.get_time() >= STICK_RESET) {
        m_movement_reset.stop();
        m_x_axis = 0.f, m_y_axis = 0.f;
    }
}

void ElementAnalogStick::write_to_file(ccl_config* cfg, SDL_Point* default_dim, uint8_t &flags)
{
    element_texture::write_to_file(cfg, default_dim, flags);
    auto comment = "Analog stick side of " + m_id;
    cfg->add_int(m_id + CFG_SIDE, comment, static_cast<int>(m_stick), true);
    comment = "Analog stick radius of " + m_id;
    cfg->add_int(m_id + CFG_STICK_RADIUS, comment, static_cast<int>(m_radius), true);

    flags |= FLAG_GAMEPAD | (m_stick == SIDE_LEFT ? FLAG_LEFT_STICK : FLAG_RIGHT_STICK);
}

void ElementAnalogStick::update_settings(dialog_new_element* dialog)
{
    element_texture::update_settings(dialog);
    m_radius = dialog->get_radius();
    m_stick = dialog->get_side();
}

void ElementAnalogStick::update_settings(dialog_element_settings* dialog)
{
    element_texture::update_settings(dialog);
}

void ElementAnalogStick::handle_event(SDL_Event* event, sdl_helper* helper)
{
    if (event->type == SDL_CONTROLLERAXISMOTION && SDL_abs(event->caxis.value) >= STICK_DEAD_ZONE) {
        if (m_stick == SIDE_LEFT) {
            if (event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
                m_x_axis = ((float) event->caxis.value) / AXIS_MAX_AMPLITUDE;
                m_movement_reset.start();
            } else if (event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
                m_y_axis = static_cast<float>(event->caxis.value) / AXIS_MAX_AMPLITUDE;
                m_movement_reset.start();
            }
        } else {
            if (event->caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX) {
                m_x_axis = static_cast<float>(event->caxis.value) / AXIS_MAX_AMPLITUDE;
                m_movement_reset.start();
            } else if (event->caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY) {
                m_y_axis = static_cast<float>(event->caxis.value) / AXIS_MAX_AMPLITUDE;
                m_movement_reset.start();
            }
        }
    } else if (event->type == SDL_CONTROLLERBUTTONDOWN || event->type == SDL_CONTROLLERBUTTONUP) {
        if (m_stick == SIDE_LEFT) {
            if (event->cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSTICK)
                m_pressed = event->cbutton.state == SDL_PRESSED;
        } else {
            if (event->cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSTICK)
                m_pressed = event->cbutton.state == SDL_PRESSED;
        }
    }
}

ElementAnalogStick* ElementAnalogStick::read_from_file(ccl_config* file, const std::string &id, SDL_Point* default_dim)
{
    const auto s = read_side(file, id);

    return new ElementAnalogStick(id, read_position(file, id), read_mapping(file, id, default_dim), s,
                                  file->get_int(id + CFG_STICK_RADIUS), read_layer(file, id));
}
