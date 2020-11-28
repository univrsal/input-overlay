/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
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

#include "../hook/uiohook_helper.hpp"
#include "element/element.hpp"
#include <map>
#include <memory>
#include <vector>

class ccl_config;

class element_data;

typedef struct gs_image_file gs_image_file_t;

class overlay {
public:
    overlay() = default;
    ~overlay();
    explicit overlay(sources::overlay_settings *settings);
    bool load();
    void unload();
    void draw(gs_effect_t *effect);
    void refresh_data();
    bool is_loaded() const { return m_is_loaded; }
    gs_image_file_t *get_texture() const { return m_image; }

private:
    bool load_cfg();
    bool load_texture();
    void unload_texture() const;
    void unload_elements();
    void load_element(const QJsonObject &obj, bool debug);

    static const char *element_type_to_string(element_type t);

    gs_image_file_t *m_image = nullptr;
    sources::overlay_settings *m_settings = nullptr;
    bool m_is_loaded = false;
    std::vector<std::unique_ptr<element>> m_elements;
    uint16_t m_track_radius{};
    uint16_t m_max_mouse_movement{};
    float m_arrow_rot = 0.f;
};
