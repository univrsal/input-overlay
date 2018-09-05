/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include "ElementTexture.hpp"
#include "../util/Util.hpp"

#define WHEEL_RESET 150

#define POS_WHEEL_NONE      0
#define POS_WHEEL_PRESSED   1
#define POS_WHEEL_UP        2
#define POS_WHEEL_DOWN      3

class ElementScrollWheel
    : public ElementTexture
{
public:
    ElementScrollWheel() : ElementTexture() { /* NO-OP */ };

    ElementScrollWheel(std::string id, SDL_Point pos, SDL_Rect mapping, uint8_t z);

    void draw(Texture * atlas, CoordinateSystem * cs, bool selected, bool alpha) override;

    void handle_event(SDL_Event * event, SDL_Helper * helper) override;

    void refresh_mappings(void);

    void update_settings(DialogNewElement * dialog) override;

    void update_settings(DialogElementSettings * dialog) override;

    static ElementScrollWheel * read_from_file(ccl_config * file, std::string id, SDL_Point * default_dim);
private:
    SDL_Rect m_mapping_pressed, m_mapping_down, m_mapping_up;
    Timer m_wheel_reset;
    bool m_pressed = false;
    bool m_up = false;
    bool m_down = false;
};
