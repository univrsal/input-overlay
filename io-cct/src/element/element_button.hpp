/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "element_texture.hpp"

class ElementButton : public element_texture
{
public:
    ElementButton() : element_texture(), m_pressed_mapping()
    {
        /* NO-OP */
    }

    ElementButton(const std::string &id, SDL_Point pos, SDL_Rect mapping, uint16_t vc, uint8_t z);

    ElementError is_valid(notifier* n, sdl_helper* h) override;

    void draw(texture* atlas, coordinate_system* cs, bool selected, bool alpha) override;

    void write_to_file(ccl_config* cfg, SDL_Point* default_dim, uint8_t &layout_flags) override;

    void update_settings(dialog_new_element* dialog) override;

    void update_settings(dialog_element_settings* dialog) override;

    void handle_event(SDL_Event* event, sdl_helper* helper) override;

    int get_vc() override
    { return m_keycode; }

    static ElementButton* read_from_file(ccl_config* file, const std::string &id, SDL_Point* default_dim);

private:
    SDL_Rect m_pressed_mapping;
    uint16_t m_keycode = 0;
    bool m_pressed = false;
};
