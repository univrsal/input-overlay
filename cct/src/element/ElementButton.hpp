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
    ElementButton() : ElementTexture(), m_pressed_mapping()
    {
        /* NO-OP */
    }

    ElementButton(const std::string& id, SDL_Point pos, SDL_Rect mapping, uint16_t vc, uint8_t z);

    ElementError is_valid(Notifier* n, SDL_Helper* h) override;

    void draw(Texture* atlas, CoordinateSystem* cs, bool selected, bool alpha) override;

    void write_to_file(ccl_config* cfg, SDL_Point* default_dim, uint8_t& layout_flags) override;

    void update_settings(DialogNewElement* dialog) override;

    void update_settings(DialogElementSettings* dialog) override;

    void handle_event(SDL_Event* event, SDL_Helper* helper) override;

    int get_vc() override { return m_keycode; }

    static ElementButton* read_from_file(ccl_config* file, const std::string& id, SDL_Point* default_dim);
private:
    SDL_Rect m_pressed_mapping;
    uint16_t m_keycode = 0;
    bool m_pressed = false;
};
