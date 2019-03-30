/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include <map>

extern "C" {
#include <graphics/image-file.h>
}

struct icon
{
    uint16_t u, v;
};

class history_icons
{
    bool m_loaded = false;
    uint16_t m_icon_count = 0;
    uint16_t m_icon_w = 0;
    uint16_t m_icon_h = 0;
    std::map<uint16_t, icon> m_icons;
    gs_image_file_t* m_icon_texture = nullptr;

    void unload_texture();

public:
    ~history_icons();

    void load_from_file(const char* cfg, const char* img);

    void draw(uint16_t vc, vec2* pos);

    uint16_t get_w() const
    { return m_icon_w; }

    uint16_t get_h() const
    { return m_icon_h; }

    bool is_loaded() const
    { return m_loaded; }
};
