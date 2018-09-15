/**
 * Created by universal on 06.09.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "ElementTexture.hpp"

class ElementTrigger
    : public ElementTexture
{
public:
    ElementTrigger() :
        ElementTexture(), m_side(), m_direction()
    {
        /* NO-OP */
    };
    ElementTrigger(const std::string& id, SDL_Point pos, SDL_Rect mapping,
                   element_side s, trigger_direction d, uint8_t z);

    ElementTrigger(const std::string& id, SDL_Point pos, SDL_Rect mapping,
                   element_side s, uint8_t z);

    void draw(Texture* atlas, CoordinateSystem* cs, bool selected, bool alpha) override;

    void write_to_file(ccl_config* cfg, SDL_Point* default_dim) override;

    void update_settings(DialogNewElement* dialog) override;

    void update_settings(DialogElementSettings* dialog) override;

    void handle_event(SDL_Event* event, SDL_Helper* helper) override;

    static ElementTrigger* read_from_file(ccl_config* file, const std::string& id, SDL_Point* default_dim);

    element_side get_side() const { return m_side; }

    trigger_direction get_direction() const { return m_direction; }

    bool get_mode() const { return m_button_mode; }
private:
    void calculate_mappings(SDL_Rect* pressed, SDL_Rect* absolute) const;

    SDL_Rect m_pressed_mapping = {};

    float m_progress = 0;
    element_side m_side;
    trigger_direction m_direction;
    bool m_button_mode = false;
};
