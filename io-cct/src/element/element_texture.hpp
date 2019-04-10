/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "element.hpp"

class element_texture : public element
{
public:
    element_texture()
    {
        /* NO-OP */
    };

    element_texture(const std::string &id, SDL_Point pos, SDL_Rect mapping, uint8_t z);

    ElementError is_valid(notifier* n, sdl_helper* h) override;

    void draw(texture* atlas, coordinate_system* cs, bool selected, bool alpha) override;

    void write_to_file(ccl_config* cfg, SDL_Point* default_dim, uint8_t &layout_flags) override;

    void update_settings(dialog_new_element* dialog) override;

    void update_settings(dialog_element_settings* dialog) override;

    void handle_event(SDL_Event* event, sdl_helper* helper) override
    {
        /* NO-OP */
    };

    static element_texture* read_from_file(ccl_config* file, const std::string &id, SDL_Point* default_dim);

protected:
    element_texture(element_type t, const std::string &id, SDL_Point pos, SDL_Rect mapping, uint8_t z);
};
