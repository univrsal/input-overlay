/**
 * Created by univrsal on 23.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "../util/SDL_Helper.hpp"
#include "elements/GuiElement.hpp"
#include "elements/Label.hpp"
#include <string>
#include <vector>
#include <memory>

#define DIALOG_DRAGGABLE        1 << 0 /* Allow moving by dragging the title bar*/
#define DIALOG_CENTERED         1 << 1 /* Always center on window resize */
#define DIALOG_TEXTINPUT        1 << 2 /* Hint SDL to read text input */
#define DIALOG_TOP_MOST         1 << 3 /* Draws background tint */
#define DIALOG_FLUID            1 << 4 /* Dialog adapts to window size. Includes DIALOG_CENTERED */

#define FLUID_BORDER            32

#define ACTION_FOCUSED          -1
#define ACTION_UNFOCUSED        -2
#define ACTION_OK               -3
#define ACTION_CANCEL           -4

class SDL_Helper;

class GuiElement;

class Dialog
{
public:
    Dialog(SDL_Helper* sdl, SDL_Rect size, std::string title);
    Dialog(SDL_Helper* sdl, SDL_Point size, std::string title);

    virtual ~Dialog();

    virtual void init();

    virtual void draw_background();

    virtual void draw_foreground();

    virtual void close();

    virtual bool handle_events(SDL_Event* event);

    virtual void action_performed(int8_t action_id);

    void add(GuiElement* e);

    void set_dimension(uint16_t w, uint16_t h);

    void set_flags(uint16_t flags);

    void reload_lang();

    void change_focus(int8_t id);

    SDL_Point position() const;

    SDL_Helper* helper() const;

    int get_left() const;

    int get_top() const;

    int get_right() const;

    int get_bottom() const;
protected:
    /* Tab handling */
    GuiElement* m_focused_element = nullptr;
    int8_t m_focused_element_id = 0;

    std::vector<std::unique_ptr<GuiElement>> m_screen_elements;
    std::vector<GuiElement*> m_tab_items;
    std::string m_title;


    int m_offset_x, m_offset_y{};
    bool m_is_dragging = false;
    uint16_t m_flags = 0x0000;

    SDL_Helper* m_helper;
    SDL_Rect m_dimensions{};
    SDL_Rect m_title_bar{};
};
