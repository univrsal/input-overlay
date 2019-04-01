/**
 * Created by univrsal on 30.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <utility>
#include "dialog.hpp"
#include "elements/button.hpp"
#include "elements/textbox.hpp"
#include "elements/atlas_selector.hpp"
#include "elements/checkbox.hpp"
#include "elements/combobox.hpp"
#include "../tool.hpp"
#include "../util/sdl_helper.hpp"

class sdl_helper;

class texture;

class tool;

class element;

enum element_type;

class dialog_new_element : public dialog
{
public:
    dialog_new_element(sdl_helper* sdl, const std::string &title, tool* tool, element_type type) : dialog(sdl,
                                                                                                        SDL_Point{},
                                                                                                        title)
    {
        m_tool = tool;
        m_type = type;
    }

    void init() override;

    void action_performed(int8_t action_id) override;

    void draw_background() override;

    void draw_foreground() override;

    bool handle_events(SDL_Event* event) override;

    void load_from_element(element* e);

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

    bool get_trigger_mode() const
    { return m_trigger_mode ? m_trigger_mode->get_state() : false; }

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
    void add_mouse_or_analog_stick(const char* label_text, const char* item_a, const char* item_b);

    /* Adds informational label */
    void add_info(const char* unlocalized_text);

    /* Adds game pad trigger elements */
    void add_trigger();

    /* Tracks whether or not the element name was changed*/
    std::string m_initial_name;

    uint16_t m_element_y = 0; /* Y position of the lowest gui element */
    static const uint16_t panel_w = 254;
    uint8_t m_id = 1;

    atlas_selector* m_selector = nullptr;
    tool* m_tool = nullptr;
    element_type m_type;

    textbox* m_radius = nullptr;
    /* Used for mouse movement type and analog stick side*/
    combobox* m_binary_choice = nullptr;

    combobox* m_direction = nullptr;

    textbox* m_w = nullptr;
    textbox* m_h = nullptr;
    textbox* m_u = nullptr;
    textbox* m_v = nullptr;

    textbox* m_element_id = nullptr;
    textbox* m_keycode = nullptr;
    textbox* m_z_level = nullptr;

    button* m_ok = nullptr;
    button* m_cancel = nullptr;

    checkbox* m_read_keybind = nullptr;
    checkbox* m_trigger_mode = nullptr;

    /* UV mapping */
    SDL_Rect m_selection;

    bool m_modify_mode = false;
};
