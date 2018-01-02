#include <stdio.h>
#include <stdint.h>

struct InputKey {
    unsigned char m_key_char;
    bool m_pressed;
    uint16_t texture_u, texture_v;
    uint16_t w, h;

    uint16_t x_offset; // used to center buttons that span over multiple columns
    uint16_t row, column; // On screen location (in pixels for mouse/controller layouts)
};