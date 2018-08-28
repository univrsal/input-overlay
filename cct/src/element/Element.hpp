/**
 * Created by universal on 27.08.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "../../../io-obs/util/layout_constants.hpp"
#include <string>
#include <SDL.h>

class SDL_Helper;

class Notifier;

class DialogElementSettings;

class DialogNewElement;

class CoordinateSystem;

class ccl_config;

class Texture;

/* Base class for display elements
 */
class Element
{
public:
    virtual void draw(Texture * atlas, CoordinateSystem * cs, bool selected, bool alpha) = 0;

    virtual void write_to_file(ccl_config * cfg, SDL_Point * default_dim);

    virtual SDL_Rect * get_abs_dim(CoordinateSystem * cs);

    virtual void update_settings(DialogNewElement * dialog);

    virtual void update_settings(DialogElementSettings * dialog);

    virtual bool is_valid(Notifier * n, SDL_Helper * h);

    void set_mapping(SDL_Rect r);

    void set_pos(int x, int y);

    void set_id(std::string id) { m_id = id;}

    void set_z_level(uint8_t z) { m_z_level = z; }

    uint8_t get_z_level(void) { return m_z_level; }

    std::string * get_id(void) { return &m_id; }

    int get_x() { return m_position.x; }

    int get_y() { return m_position.y; }

    int get_w() { return m_mapping.w; }

    int get_h() { return m_mapping.h; }

    int get_u() { return m_mapping.x; } 

    int get_v() { return m_mapping.y; }  

    int get_vc() { return m_keycode; }

    ElementType get_type(void) { return m_type; }

    SDL_Rect * get_mapping(void) { return &m_mapping; }

    virtual void handle_event(SDL_Event * event) = 0;

    static Element * read_from_file(ccl_config * file, std::string id, ElementType t, SDL_Point * default_dim);

    static bool valid_type(int t);

protected:
    Element(ElementType t, std::string id, SDL_Point pos, uint8_t z);

    static SDL_Rect element_read_mapping(ccl_config * file, std::string id, SDL_Point * default_dim);
    static SDL_Point element_read_position(ccl_config * file, std::string id);
    static uint8_t element_read_layer(ccl_config * file, std::string id);

    ElementType m_type;
    SDL_Point m_position; /* Final position in overlay */
    SDL_Rect m_mapping; /* Texture mappings */
    SDL_Rect m_dimensions_scaled;

    uint8_t m_scale = 0; /* Currently used scale factor */

    uint8_t m_z_level = 0; /* Determines draw and selection order */
    uint16_t m_keycode = 0;
    std::string m_id;
};
