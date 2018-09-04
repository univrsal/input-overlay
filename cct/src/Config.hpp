/**
 * Created by universal on 27.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "../../io-obs/util/layout_constants.hpp"
#include "util/CoordinateSystem.hpp"
#include "element/Element.hpp"
#include <string>
#include <SDL.h>
#include <memory>
#include <vector>

class ccl_config;

class Notifier;

class CoordinateSystem;

class DialogElementSettings;

class SDL_Helper;

class Texture;

class Config
{
public:
    Config(const char * texture, const char * config, SDL_Point def_dim, SDL_Point space, SDL_Helper * h, DialogElementSettings * s);
    ~Config();

    void draw_elements(void);

    void handle_events(SDL_Event * e);

    std::string m_texture_path;
    std::string m_config_path;

    void write_config(Notifier * n);
    void read_config(Notifier * n);

    Texture * get_texture(void);

    SDL_Point get_default_dim(void);

    void queue_delete(uint16_t id)
    {
        if (!m_elements.empty())
            m_element_to_delete = id;
    }

    std::vector<std::unique_ptr<Element>> m_elements;

    Element * selected() { return m_selected; }
    uint16_t selecte_id() { return m_selected_id; }

    void reset_selection(void);
private:
    /* Move selected elements*/
    void move_elements(int new_x, int new_y);
    inline void move_element(int mouse_x, int mouse_y);

    inline bool is_rect_in_rect(const SDL_Rect * a, const SDL_Rect * b);

    int16_t m_element_to_delete = -1;
    int16_t m_selected_id = -1;
    Element * m_selected = nullptr;

    CoordinateSystem m_cs;

    SDL_Helper * m_helper = nullptr;
    Texture * m_atlas = nullptr;
    DialogElementSettings * m_settings = nullptr;

    bool m_in_single_selection = false; /* Flag for dragging single element */
    SDL_Point m_drag_offset;

    SDL_Point m_default_dim;
    SDL_Point m_offset;

    /* Selection stuff */
    std::vector<uint16_t> m_selected_elements;
    SDL_Rect m_total_selection; /* Rectangle fitting all selected elements, oriented to origin */
    SDL_Rect m_temp_selection; /* Indicator of current rectangle selected by mouse */
    SDL_Point m_selection_start;

    bool m_in_multi_selection = false;
    bool m_dragging_elements = false; /* Flag for dragging entire selection*/
};
