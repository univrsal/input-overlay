/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "element_texture.hpp"
#include "../util/util.hpp"

#define WHEEL_RESET 150

#define POS_WHEEL_NONE      0
#define POS_WHEEL_PRESSED   1
#define POS_WHEEL_UP        2
#define POS_WHEEL_DOWN      3

class ElementScrollWheel : public element_texture
{
public:
    ElementScrollWheel() : element_texture()
    {
        /* NO-OP */
    };

    ElementScrollWheel(const std::string &id, SDL_Point pos, SDL_Rect mapping, uint8_t z);

    void draw(texture* atlas, coordinate_system* cs, bool selected, bool alpha) override;

    void handle_event(SDL_Event* event, sdl_helper* helper) override;

    void refresh_mappings();

    void update_settings(dialog_new_element* dialog) override;

    void update_settings(dialog_element_settings* dialog) override;

    static ElementScrollWheel* read_from_file(ccl_config* file, const std::string &id, SDL_Point* default_dim);

private:
    SDL_Rect m_mapping_pressed = {}, m_mapping_down = {}, m_mapping_up = {};
    Timer m_wheel_reset;
    bool m_pressed = false;
    bool m_up = false;
    bool m_down = false;
};
