/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "ElementTexture.hpp"

class ElementButton
    : public ElementTexture
{
public:
    ElementButton(std::string id, SDL_Point pos, SDL_Rect mapping, uint16_t vc, uint8_t z);

    void draw(Texture * atlas, CoordinateSystem * cs, bool selected, bool alpha) override;

    void write_to_file(ccl_config * cfg, SDL_Point * default_dim) override;

    void update_settings(DialogNewElement * dialog) override;

    void update_settings(DialogElementSettings * dialog) override;

    void handle_event(SDL_Event * event, SDL_Helper * helper) override;

    int get_vc() override { return 0; }

    static ElementButton * read_from_file(ccl_config * file, std::string id, SDL_Point * default_dim);
private:
    uint16_t m_keycode = 0;
    bool m_pressed = false;
};
