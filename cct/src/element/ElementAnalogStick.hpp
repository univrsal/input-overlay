/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#define STICK_RESET 500

#include "ElementTexture.hpp"
#include "../util/Util.hpp"

class ElementAnalogStick
    : public ElementTexture
{
public:
    ElementAnalogStick() : ElementTexture() { /* NO-OP */ };

    ElementAnalogStick(std::string id, SDL_Point pos, SDL_Rect mapping, ElementSide side, uint8_t radius, uint8_t z);

    SDL_Rect * get_abs_dim(CoordinateSystem * cs) override;

    ElementError is_valid(Notifier * n, SDL_Helper * h) override;

    void draw(Texture * atlas, CoordinateSystem * cs, bool selected, bool alpha) override;

    void write_to_file(ccl_config * cfg, SDL_Point * default_dim) override;

    void update_settings(DialogNewElement * dialog) override;

    void update_settings(DialogElementSettings * dialog) override;

    void handle_event(SDL_Event * event, SDL_Helper * helper) override;

    ElementSide get_stick(void) { return m_stick; }

    uint16_t get_radius(void) { return m_radius; }

    static ElementAnalogStick * read_from_file(ccl_config * file, const std::string& id, SDL_Point * default_dim);
private:
    SDL_Rect m_static_scaled; /* Position without input movement for display */
    ElementSide m_stick;
    bool m_pressed = false;
    float m_x_axis = 0;
    float m_y_axis = 0;
    uint16_t m_radius = 0;

    Timer m_movement_reset;
};
