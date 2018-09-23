/**
* Created by univrsal on 14.07.2017.
* This file is part of input-overlay which is licensed
* under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
* github.com/univrsal/input-overlay
*/

#pragma once

#include <SDL.h>

#define DEBUG_DRAW_OUTLINE 0

#define ELEMENT_ABSOLUTE_POSITION 1 << 10 /* Element will not align to parent dialog position */
#define ELEMENT_UNLOCALIZED 1 << 11 /* The element will not look up localized strings */

class Dialog;

class SDL_Helper;

class GuiElement
{
public:
    virtual ~GuiElement();

    virtual void draw_background() = 0;

    virtual void draw_foreground();

    virtual void close();

    virtual bool handle_events(SDL_Event* event, bool was_handled) = 0;

    virtual bool can_select(); /* Tab selection */

    virtual void select_state(bool state);

    virtual void resize();

    virtual void refresh(); /* Update localization */

    virtual bool is_mouse_over(const int& x, const int& y);

    virtual bool is_mouse_over(const SDL_Point* p = nullptr);

    virtual void set_pos(int x, int y);

    virtual uint8_t get_cursor();

    Dialog* get_parent() const;

    SDL_Helper* get_helper() const;

    SDL_Rect* get_dimensions();

    void set_dim(SDL_Rect r);

    int8_t get_id() const;

    virtual void init(Dialog* parent, SDL_Rect dim, int8_t id);

    void set_flags(uint8_t flags);

    int get_left() const;

    int get_top() const;

    int get_right() const;

    int get_bottom() const;

    int get_width() const;

    int get_height() const;
protected:
    Dialog* m_parent_dialog = nullptr;
    SDL_Rect m_dimensions = {}; /* Width/Height and absolute position */
    SDL_Point m_position = {}; /* Position relative to dialog*/
    int8_t m_element_id = 0;
    uint16_t m_flags = 0x0000;
};
