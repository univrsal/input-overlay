/**
 * Created by univrsal on 30.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <stdlib.h>
#include "Dialog.hpp"
#include "elements/Button.hpp"
#include "elements/Textbox.hpp"
#include "elements/AtlasSelector.hpp"
#include "elements/Checkbox.hpp"
#include "elements/Combobox.hpp"
#include "../Config.hpp"
#include "../Tool.hpp"
#include "../util/SDL_Helper.hpp"
#include "../util/Texture.hpp"
#include "../util/CoordinateSystem.hpp"
#include "../util/Constants.hpp"

class SDL_Helper;

class Texture;

class Tool;

class Element;

enum ElementType;

class DialogNewElement : public Dialog
{
public:
    DialogNewElement(SDL_Helper * sdl, std::string title, Tool * tool, ElementType type)
        : Dialog(sdl, SDL_Point {}, title)
    {
        m_tool = tool;
        m_type = type;
    }

    void close(void) override;

    void init() override;

    void action_performed(int8_t action_id) override;

    void draw_background(void) override;

    void draw_foreground(void) override;

    bool handle_events(SDL_Event * event) override;

    void load_from_element(Element * e);

    ElementType get_type(void);

    SDL_Rect get_selection(void);

    uint16_t get_vc(void);

    uint8_t get_z_level(void);

    ElementSide get_side(void);

    MouseMovementType get_mouse_type(void);

    uint8_t get_radius(void);

    const std::string * get_id(void);

    void set_default_dim(int w, int h);

    bool get_trigger_mode(void) { return m_trigger_mode ? m_trigger_mode->get_state() : false; }

private:
    void handle_error(ElementError e);

    /* Adds default elements*/
    void add_selection_elements(void);

    /* Adds key code elements */
    void add_keycode_elements(void);

    /* Adds text box for element id*/
    void add_element_id(void);

    /* Adds layering elements */
    void add_z_level(void);

    /* Adds analog stick or mouse movement elements*/
    void add_mouse_or_analog_stick(const char * label, const char * item_a, const char * item_b);

    /* Adds informational label */
    void add_info(const char * unlocalized_text);

    /* Adds game pad trigger elements */
    void add_trigger(void);

    /* Tracks whether or not the element name was changed*/
    std::string m_initial_name;

    uint16_t m_element_y = 0;  /* Y position of the lowest gui element */
    static const uint16_t panel_w = 254;
    uint16_t m_id = 1;

    AtlasSelector * m_selector = nullptr;
    Tool * m_tool = nullptr;
    ElementType m_type;

    Textbox * m_radius = nullptr;
    /* Used for mouse movement type and analog stick side*/
    Combobox * m_binary_choice = nullptr;

    Combobox * m_direction = nullptr;

    Textbox * m_w = nullptr;
    Textbox * m_h = nullptr;
    Textbox * m_u = nullptr;
    Textbox * m_v = nullptr;

    Textbox * m_element_id = nullptr;
    Textbox * m_keycode = nullptr;
    Textbox * m_z_level = nullptr;

    Button * m_ok = nullptr;
    Button * m_cancel = nullptr;

    Checkbox * m_read_keybind = nullptr;
    Checkbox * m_trigger_mode = nullptr;

    /* UV mapping */
    SDL_Rect m_selection;

    SDL_Point m_default_dim;

    bool m_modify_mode = false;
};
