/**
 * Created by universal on 11.09.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "element_texture.hpp"

class ElementDPad : public element_texture
{
public:
    ElementDPad()
    {
        /* NO-OP */
    };

    ElementDPad(const std::string &id, const SDL_Point pos, const SDL_Rect mapping, const uint8_t z) : element_texture(
            id, pos, mapping, z)
    { /* NO-OP */ }

    void draw(texture* atlas, coordinate_system* cs, bool selected, bool alpha) override;

    void handle_event(SDL_Event* event, sdl_helper* helper) override;

    static ElementDPad* read_from_file(ccl_config* file, const std::string &id, SDL_Point* default_dim);

    void write_to_file(ccl_config* cfg, SDL_Point* default_dim, uint8_t &layout_flags) override;

private:
    dpad_direction m_dir = DPAD_CENTER;
    int8_t m_last_button = SDL_CONTROLLER_BUTTON_INVALID;
};
