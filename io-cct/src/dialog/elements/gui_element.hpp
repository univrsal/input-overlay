/**
* Created by univrsal on 14.07.2017.
* This file is part of input-overlay which is licensed
* under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
* github.com/univrsal/input-overlay
*/

#pragma once

#include <SDL.h>

#define DEBUG_DRAW_OUTLINE 0

class dialog;

class sdl_helper;

enum element_flags
{
    TEXTBOX_NUMERIC = 1 << 0,
    TEXTBOX_HEX = 1 << 1,
    TEXTBOX_NO_SPACE = 1 << 2,
    TEXTBOX_KEYBIND = 1 << 3,
    TEXTBOX_DROP_FILE = 1 << 4,
    TEXTBOX_ALPHA_NUMERIC = 1 << 5,
    ELEMENT_FOCUSED = 1 << 6,
    ELEMENT_ENABLED = 1 << 7,
    ELEMENT_ABSOLUTE_POSITION = 1 << 10,    /* Element will not align to parent dialog position */
    ELEMENT_UNLOCALIZED = 1 << 11           /* The element will not look up localized strings   */
};

enum actions
{
    ACTION_OK = -127,
    ACTION_CANCEL,
    ACTION_FOCUSED,
    ACTION_UNFOCUSED,
    ACTION_FILE_DROPPED,
    ACTION_COMBO_ITEM_SELECTED,
    ACTION_HELP_BUTTON,
    ACTION_NEW_ELEMENT,
    ACTION_SAVE_CONFIG,
    ACTION_DEL_ELEMENT,
    ACTION_MOD_ELEMENT,
    ACTION_TEXT_TYPED
};

class gui_element
{
public:
    virtual ~gui_element();

    virtual void draw_background() = 0;

    virtual void draw_foreground();

    virtual void close();

    virtual bool handle_events(SDL_Event* event, bool was_handled) = 0;

    virtual bool can_select(); /* Tab selection */

    virtual void select_state(bool state);

    virtual void resize();

    virtual void refresh(); /* Update get_localization */

    virtual bool is_mouse_over(const SDL_Point* p = nullptr);

    virtual void set_pos(int x, int y);

    virtual uint8_t get_cursor();

    dialog* get_parent() const;

    sdl_helper* get_helper() const;

    SDL_Rect* get_dimensions();

    void set_dim(SDL_Rect r);

    int8_t get_id() const;

    virtual void init(dialog* parent, SDL_Rect dim, int8_t id);

    void set_flags(uint16_t flags);

    int get_left() const;

    int get_top() const;

    int get_right() const;

    int get_bottom() const;

    int get_width() const;

    int get_height() const;

protected:
    dialog* m_parent_dialog = nullptr;
    bool m_enabled = true;
    SDL_Rect m_dimensions = {}; /* Width/Height and absolute position */
    SDL_Point m_position = {}; /* Position relative to dialog*/
    int8_t m_element_id = 0;
    uint16_t m_flags = 0x0000;
};
