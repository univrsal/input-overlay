/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include "Element.hpp"

class ElementText
    : public Element
{
public:
    ElementText() : Element()
    {
        /* NO-OP */
    }

    ElementText(const std::string& id, SDL_Point pos, uint8_t z,
        const std::string& text, bool reset);

    SDL_Rect* get_abs_dim(CoordinateSystem* cs) override;

    ElementError is_valid(Notifier* n, SDL_Helper* h) override;

    void draw(Texture* atlas, CoordinateSystem* cs, bool selected, bool alpha) override;

    void write_to_file(ccl_config* cfg, SDL_Point* default_dim) override;

    void update_settings(DialogNewElement* dialog) override;

    void handle_event(SDL_Event* event, SDL_Helper* helper) override;

    static ElementText* read_from_file(ccl_config* file, const std::string& id, SDL_Point* default_dim);

    std::string get_text() const
    {
        return m_unformatted;
    }

    bool get_reset() const
    {
        return m_reset;
    }

    void make_flags();
private:
    void format_text(SDL_Event* e);

    std::string m_unformatted;
    std::string m_formatted;

    uint8_t m_last_button = 0;
    uint8_t m_flags = 0x0;

    uint16_t m_lmb_clicks = 0;
    uint16_t m_rmb_clicks = 0;
    uint16_t m_mmb_clicks = 0;
    int16_t m_scroll_amount = 0;

    int32_t m_mouse_x = 0, m_mouse_y = 0;
    bool m_reset = false;
};
