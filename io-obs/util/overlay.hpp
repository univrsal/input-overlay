/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#ifdef LINUX
#include <stdint.h>
#endif
#include <memory>
#include <vector>
#include <map>
#include "element/element.hpp"

#include "../hook/hook_helper.hpp"

class ccl_config;
class element_data;

typedef struct gs_image_file gs_image_file_t;

//namespace Data {

class overlay
{
public:
    overlay() = default;

    ~overlay();

    overlay(sources::overlay_settings* settings);

    bool load();

    void unload();

    void draw(gs_effect_t* effect);

    void refresh_data();

    bool is_loaded() const
    {
        return m_is_loaded;
    }

    gs_image_file_t* get_texture() const
    {
        return m_image;
    }

private:
    bool load_cfg();
    bool load_texture();
    void unload_texture() const;
    void unload_elements();
    void load_element(ccl_config* cfg, const std::string& id, bool debug);

    static const char* element_type_to_string(element_type t);

    gs_image_file_t* m_image = nullptr;

    sources::overlay_settings* m_settings = nullptr;

    bool m_is_loaded = false;
    std::vector<std::unique_ptr<element>> m_elements;
    std::map<uint16_t, std::unique_ptr<element_data>> m_data;

    uint16_t m_track_radius{};
    uint16_t m_max_mouse_movement{};
    float m_arrow_rot = 0.f;
};
