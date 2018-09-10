/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once
#include "Element.hpp"

class ElementText
    : Element
{
public:
    ElementText()
    {
        /* NO-OP */
    };

    ElementText(ElementType t, std::string id, SDL_Point pos, std::string text, uint8_t z);

private:
    std::string m_text;
};
