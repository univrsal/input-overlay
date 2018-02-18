#ifndef LAYOUT_HPP
#define LAYOUT_HPP

#ifdef LINUX
#include <stdint.h>
#endif
#include <vector>

#include "layouttype.hpp"

struct InputKey {
    uint16_t m_key_code;
    bool m_pressed = false;
    uint16_t texture_u, texture_v;
    uint16_t w, h;

    uint16_t x_offset; // used to center buttons that span over multiple columns
    uint16_t row, column; // On screen location (in pixels for mouse/controller layouts)
};

struct OverlayLayout {
    LayoutType m_type;
    uint8_t m_key_count;
    int8_t m_key_space_v, m_key_space_h;
    uint8_t m_btn_w, m_btn_h;
    uint16_t m_w, m_h;
    uint8_t m_rows, m_cols;
    uint16_t texture_v_space;
    bool m_is_loaded = false;
    bool m_mouse_movement = false;
    bool m_use_arrow = false;
    std::vector<InputKey> m_keys;

    uint16_t m_track_radius;
    uint16_t m_max_mouse_movement;
    float m_arrow_rot = 0.f;
};

#endif // LAYOUT_HPP