/**
 * Created by univrsal on 30.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <utility>
#include "Dialog.hpp"
#include "elements/Button.hpp"
#include "elements/Textbox.hpp"
#include "elements/AtlasSelector.hpp"
#include "elements/Checkbox.hpp"
#include "elements/Combobox.hpp"
#include "../Tool.hpp"
#include "../util/SDL_Helper.hpp"

class SDL_Helper;

class Texture;

class Tool;

class Element;

enum element_type;

class DialogNewElement : public Dialog
{
public:
    DialogNewElement(SDL_Helper* sdl, const std::string& title, Tool* tool, element_type type)
        : Dialog(sdl, SDL_Point{}, title)
    {
        m_tool = tool;
        m_type = type;
    }

    void init() override;

    void action_performed(int8_t action_id) override;

    void draw_background() override;

    void draw_foreground() override;

    bool handle_events(SDL_Event* event) override;

    void load_from_element(Element* e);

    element_type get_type() const;

    SDL_Rect get_selection() const;

    uint16_t get_vc() const;

    uint8_t get_z_level() const;

    element_side get_side() const;

    trigger_direction get_direction() const;

    mouse_movement_type get_mouse_type() const;

    uint8_t get_radius() const;

    const std::string* get_id() const;

    void set_default_dim(int w, int h);

    bool get_trigger_mode() const { return m_trigger_mode ? m_trigger_mode->get_state() : false; }

    const std::string* get_text() const;

    bool get_text_reset() const;
private:
    void handle_error(ElementError e) const;

    /* Adds default elements*/
    void add_selection_elements();

    /* Adds key code elements */
    void add_keycode_elements();

    /* Adds text box for element id*/
    void add_element_id();

    /* Adds layering elements */
    void add_z_level();

    /* Adds analog stick or mouse movement elements*/
    void add_mouse_or_analog_stick(const char* label, const char* item_a, const char* item_b);

    /* Adds informational label */
    void add_info(const char* unlocalized_text);

    /* Adds game pad trigger elements */
    void add_trigger();

    /* Adds text elements*/
    void add_text();

    /* Tracks whether or not the element name was changed*/
    std::string m_initial_name;

    uint16_t m_element_y = 0; /* Y position of the lowest gui element */
    static const uint16_t panel_w = 254;
    uint8_t m_id = 1;

    AtlasSelector* m_selector = nullptr;
    Tool* m_tool = nullptr;
    element_type m_type;

    Textbox* m_radius = nullptr;
    /* Used for mouse movement type and analog stick side*/
    Combobox* m_binary_choice = nullptr;

    Combobox* m_direction = nullptr;

    Textbox* m_w = nullptr;
    Textbox* m_h = nullptr;
    Textbox* m_u = nullptr;
    Textbox* m_v = nullptr;

    Textbox* m_element_id = nullptr;
    Textbox* m_keycode = nullptr;
    Textbox* m_z_level = nullptr;
    Textbox* m_text = nullptr;

    Button* m_ok = nullptr;
    Button* m_cancel = nullptr;

    Checkbox* m_read_keybind = nullptr;
    Checkbox* m_trigger_mode = nullptr;
    Checkbox* m_text_reset = nullptr;

    /* UV mapping */
    SDL_Rect m_selection;

    bool m_modify_mode = false;
};
