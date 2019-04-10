/**
 * Created by universal on 06.09.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "element_texture.hpp"

class element_trigger : public element_texture
{
public:
    element_trigger() : element_texture(), m_side(), m_direction()
    {
        /* NO-OP */
    };

    element_trigger(const std::string &id, SDL_Point pos, SDL_Rect mapping, element_side s, trigger_direction d,
                   uint8_t z);

    element_trigger(const std::string &id, SDL_Point pos, SDL_Rect mapping, element_side s, uint8_t z);

    void draw(texture* atlas, coordinate_system* cs, bool selected, bool alpha) override;

    void write_to_file(ccl_config* cfg, SDL_Point* default_dim, uint8_t &layout_flags) override;

    void update_settings(dialog_new_element* dialog) override;

    void update_settings(dialog_element_settings* dialog) override;

    void handle_event(SDL_Event* event, sdl_helper* helper) override;

    static element_trigger* read_from_file(ccl_config* file, const std::string &id, SDL_Point* default_dim);

    element_side get_side() const
    { return m_side; }

    trigger_direction get_direction() const
    { return m_direction; }

    bool get_mode() const
    { return m_button_mode; }

private:
    void calculate_mappings(SDL_Rect* pressed, SDL_Rect* absolute) const;

    SDL_Rect m_pressed_mapping = {};

    float m_progress = 0;
    element_side m_side;
    trigger_direction m_direction;
    bool m_button_mode = false;
};
