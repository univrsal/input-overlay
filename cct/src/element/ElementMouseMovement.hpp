/**
 * Created by universal on 05.09.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "ElementTexture.hpp"

class ElementMouseMovement
    : public ElementTexture
{
public:
    ElementMouseMovement() : ElementTexture(), m_type()
    {
        /* NO-OP */
    };

    ElementMouseMovement(const std::string& id, SDL_Point pos, SDL_Rect mapping, mouse_movement_type type, uint16_t radius,
                         uint8_t z);

    ElementError is_valid(Notifier* n, SDL_Helper* h) override;

    void write_to_file(ccl_config* cfg, SDL_Point* default_dim, uint8_t& layout_flags) override;

    void update_settings(DialogNewElement* dialog) override;

    mouse_movement_type get_mouse_type() const;

    static ElementMouseMovement* read_from_file(ccl_config* file, const std::string& id, SDL_Point* default_dim);
    uint16_t get_radius() const;
private:
    mouse_movement_type m_type;
    uint16_t m_radius = 0;
};
