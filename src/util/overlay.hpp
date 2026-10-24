/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.cc/alex/input-overlay
 * Copyright 2025 univrsal <uni@vrsal.xyz>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#pragma once

#ifdef LINUX
#include <stdint.h>
#endif

#include "element/element.hpp"
#include <memory>
#include <vector>
extern "C" {
#include <graphics/image-file.h>
}

#if LIBOBS_API_MAJOR_VER >= 33
typedef gs_image_file_ex_t overlay_image_file_t;
#else
typedef gs_image_file4_t overlay_image_file_t;
#endif

class overlay {
public:
    overlay() = default;
    ~overlay();
    explicit overlay(sources::overlay_settings *settings);
    bool load();
    void unload();
    bool load_texture();
    void draw(gs_effect_t *effect);
    void tick(float seconds);
    void refresh_data();
    bool is_loaded() const { return m_is_loaded; }
    overlay_image_t *get_texture() const
    {
#if LIBOBS_API_MAJOR_VER >= 33
        return m_image;
#else
        return m_image ? &m_image->image3.image2.image : nullptr;
#endif
    }

private:
    bool load_cfg();
    void unload_texture();
    void unload_elements();
    void load_element(const QJsonObject &obj, bool debug, bool needs_conversion = false);

    static const char *element_type_to_string(element_type t);

    overlay_image_file_t *m_image = nullptr;
    sources::overlay_settings *m_settings = nullptr;
    bool m_is_loaded = false;
    std::vector<std::unique_ptr<element>> m_elements;
    uint16_t m_track_radius{};
    uint16_t m_max_mouse_movement{};
    float m_arrow_rot = 0.f;
};
