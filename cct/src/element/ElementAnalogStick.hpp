/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "ElementTexture.hpp"

class ElementAnalogStick
    : public ElementTexture
{
public:
    ElementAnalogStick(std::string id, SDL_Point pos, SDL_Rect mapping, AnalogStick side, uint8_t radius, uint8_t z);

    virtual SDL_Rect * get_abs_dim(CoordinateSystem * cs) override;

    void draw(Texture * atlas, CoordinateSystem * cs, bool selected, bool alpha) override;

    void write_to_file(ccl_config * cfg, SDL_Point * default_dim) override;

    void update_settings(DialogNewElement * dialog) override;

    void update_settings(DialogElementSettings * dialog) override;

    void handle_event(SDL_Event * event, SDL_Helper * helper) override;

    AnalogStick get_stick(void) { return m_stick; }

    static ElementAnalogStick * read_from_file(ccl_config * file, std::string id, SDL_Point * default_dim);
private:
    AnalogStick m_stick;
    bool m_pressed = false;
    float m_x_axis = 0;
    float m_y_axis = 0;
    uint8_t m_radius = 10;
};
