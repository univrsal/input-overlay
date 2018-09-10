/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "ElementText.hpp"
#include "ElementAnalogStick.hpp"
#include "../dialog/DialogNewElement.hpp"
#include "../util/CoordinateSystem.hpp"
#include "../../../ccl/ccl.hpp"
#include "../util/Util.hpp"

ElementText::ElementText(const std::string& id, const SDL_Point pos, const uint8_t z,
    const std::string& text, const bool reset)
    : Element(TEXT, id, pos, z)
{
    m_unformatted = text;
    make_flags();
    m_reset = reset;
}

SDL_Rect * ElementText::get_abs_dim(CoordinateSystem * cs)
{
    const auto temp = cs->get_helper()->util_text_dim(&m_formatted, FONT_WSTRING);
    m_mapping.w = temp.w;
    m_mapping.h = temp.h;
    return Element::get_abs_dim(cs);
}

ElementError ElementText::is_valid(Notifier * n, SDL_Helper * h)
{
    auto error = Element::is_valid(n, h);
    if (error == VALID && m_unformatted.empty())
        error = TEXT_EMPTY;
    return error;
}

void ElementText::draw(Texture * atlas, CoordinateSystem * cs, const bool selected, bool alpha)
{
    get_abs_dim(cs);
    SDL_GetGlobalMouseState(&m_mouse_x, &m_mouse_y);
    
    if (!m_formatted.empty())
    {
        cs->get_helper()->util_text(&m_formatted,
            m_dimensions_scaled.x, m_dimensions_scaled.y,
            cs->get_helper()->palette()->white(), FONT_WSTRING, m_scale);
    }

    if (selected)
        cs->get_helper()->util_draw_rect(&m_dimensions_scaled, cs->get_helper()->palette()->red());
}

void ElementText::write_to_file(ccl_config * cfg, SDL_Point * default_dim)
{
    Element::write_to_file(cfg, default_dim);

    /* Save text and reset state */
    auto comment = "Unformatted text of " + m_id;
    cfg->add_string(m_id + CFG_TEXT, comment, m_unformatted, true);

    comment = "Whether to reset statistics for " + m_id;
    cfg->add_bool(m_id + CFG_TEXT_RESET, comment, m_reset, true);
}

void ElementText::update_settings(DialogNewElement * dialog)
{
    Element::update_settings(dialog);
    m_unformatted = *dialog->get_text();
    m_reset = dialog->get_text_reset();
    make_flags();
}

void ElementText::handle_event(SDL_Event * event, SDL_Helper * helper)
{
    if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        if (m_reset)
        {
            m_scroll_amount = 0;
            if (event->button.button != m_last_button)
            {
                m_lmb_clicks = 0;
                m_mmb_clicks = 0;
                m_rmb_clicks = 0;
            }
        }

        switch(event->button.button)
        {
        case SDL_BUTTON_LEFT:
            m_lmb_clicks++;
            break;
        case SDL_BUTTON_RIGHT:
            m_rmb_clicks++;
            break;
        case SDL_BUTTON_MIDDLE:
            m_mmb_clicks++;
            break;
        default: ;
        }

        m_last_button = event->button.button;
    }
    else if (event->type == SDL_MOUSEWHEEL)
    {
        if (event->wheel.y > 0) /* UP */
        {
            if (m_scroll_amount < 0)
                m_scroll_amount = 0;
        }
        else
        {
            if (m_scroll_amount > 0)
                m_scroll_amount = 0;
        }

        m_scroll_amount += event->wheel.y;
    }

    format_text(event);
}

ElementText * ElementText::read_from_file(ccl_config * file, const std::string & id, SDL_Point * default_dim)
{
    const auto unformatted = file->get_string(id + CFG_TEXT);
    const auto reset = file->get_bool(id + CFG_TEXT_RESET);
    return new ElementText(id, read_position(file, id),
        read_layer(file, id), unformatted, reset);
}

void ElementText::make_flags()
{
    if (m_unformatted.find(TEXT_FORMAT_WHEEL_AMOUNT) != std::string::npos)
        m_flags |= TEXT_FORMAT_WHEEL_FLAG;

    if (m_unformatted.find(TEXT_FORMAT_LMB_CLICKS) != std::string::npos)
        m_flags |= TEXT_FORMAT_LMB_FLAG;

    if (m_unformatted.find(TEXT_FORMAT_RMB_CLICKS) != std::string::npos)
        m_flags |= TEXT_FORMAT_RMB_FLAG;

    if (m_unformatted.find(TEXT_FORMAT_MMB_CLICKS) != std::string::npos)
        m_flags |= TEXT_FORMAT_MMB_FLAG;

    if (m_unformatted.find(TEXT_FORMAT_MOUSE_X) != std::string::npos)
        m_flags |= TEXT_FORMAT_MOUSE_X_FLAG;

    if (m_unformatted.find(TEXT_FORMAT_MOUSE_Y) != std::string::npos)
        m_flags |= TEXT_FORMAT_MOUSE_Y_FLAG;
}

void ElementText::format_text(SDL_Event* e)
{
    m_formatted = m_unformatted;
   
    if(m_flags & TEXT_FORMAT_LMB_FLAG)
    {
        char buff[6];
        SDL_uitoa(m_lmb_clicks, buff, 10);
        util::replace(m_formatted, TEXT_FORMAT_LMB_CLICKS, buff);
    }

    if (m_flags & TEXT_FORMAT_RMB_FLAG)
    {
        char buff[6];
        SDL_uitoa(m_rmb_clicks, buff, 10);
        util::replace(m_formatted, TEXT_FORMAT_RMB_CLICKS, buff);
    }

    if (m_flags & TEXT_FORMAT_MMB_FLAG)
    {
        char buff[6];
        SDL_uitoa(m_mmb_clicks, buff, 10);
        util::replace(m_formatted, TEXT_FORMAT_MMB_CLICKS, buff);
    }

    if (m_flags & TEXT_FORMAT_WHEEL_FLAG)
    {
        char buff[7];
        SDL_itoa(m_scroll_amount, buff, 10);
        util::replace(m_formatted, TEXT_FORMAT_WHEEL_AMOUNT, buff);
    }

    if (m_flags & TEXT_FORMAT_MOUSE_X_FLAG)
    {
        char buff[12];
        SDL_itoa(m_mouse_x, buff, 10);
        util::replace(m_formatted, TEXT_FORMAT_MOUSE_X, buff);
    }

    if (m_flags & TEXT_FORMAT_MOUSE_Y_FLAG)
    {
        char buff[12];
        SDL_itoa(m_mouse_y, buff, 10);
        util::replace(m_formatted, TEXT_FORMAT_MOUSE_Y, buff);
    }

}
