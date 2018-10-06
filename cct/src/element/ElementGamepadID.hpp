/**
 * Created by universal on 06.09.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "ElementTexture.hpp"

class ElementGamepadID
    : public ElementTexture
{
public:
    ElementGamepadID() : ElementTexture()
    {
        /* NO-OP */
    };
    ElementGamepadID(std::string id, SDL_Point pos, SDL_Rect mapping, uint8_t z);

    void ElementGamepadID::draw(Texture* atlas, CoordinateSystem* cs, bool selected, bool alpha) override;

    void handle_event(SDL_Event* event, SDL_Helper* helper) override;

    static ElementGamepadID* read_from_file(ccl_config* file, const std::string& id, SDL_Point* default_dim);

    void write_to_file(ccl_config* cfg, SDL_Point* default_dim, uint8_t& layout_flags) override;
private:
    uint8_t m_last_gamepad_id = 0;
};
