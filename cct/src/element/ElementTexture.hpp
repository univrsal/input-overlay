/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "Element.hpp"

class ElementTexture
    : public Element
{
public:
    ElementTexture()
    {
        /* NO-OP */
    };
    ElementTexture(
    const const std::string& id, SDL_Point pos, SDL_Rect mapping, uint8_t z);

    ElementError is_valid(Notifier* n, SDL_Helper* h) override;

    void draw(Texture* atlas, CoordinateSystem* cs, bool selected, bool alpha) override;

    void write_to_file(ccl_config* cfg, SDL_Point* default_dim) override;

    void update_settings(DialogNewElement* dialog) override;

    void update_settings(DialogElementSettings* dialog) override;

    void handle_event(SDL_Event* event, SDL_Helper* helper) override
    {
        /* NO-OP */
    };

    static ElementTexture* read_from_file(ccl_config* file, const std::string& id, SDL_Point* default_dim);

protected:
    ElementTexture(element_type t, const std::string& id, SDL_Point pos, SDL_Rect mapping, uint8_t z);
};
