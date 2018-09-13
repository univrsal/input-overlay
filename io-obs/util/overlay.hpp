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
#include <vector>
#include <map>
#include "element/element.hpp"

extern "C" {
#include <graphics/image-file.h>
}

#include "../../ccl/ccl.hpp"
#include "../hook/hook_helper.hpp"

//namespace Data {

class overlay
{
public:
    overlay() = default;

    ~overlay();

    overlay(const std::string& ini, const std::string& texture);

    bool load(const std::string& cfg_path, const std::string& texture_path);
    void unload();

    void draw(gs_effect_t* effect);

    bool is_loaded() const
    {
        return m_is_loaded;
    }

    gs_image_file_t* get_texture() const
    {
        return m_image;
    }

    int get_width() const { return m_w; }
    int get_height() const { return m_h; }
private:
    bool load_cfg(const std::string& path);
    bool load_texture(const std::string& path);
    void unload_texture() const;
    void unload_elements();
    void load_element(ccl_config* cfg, const std::string& id);

    gs_image_file_t* m_image = nullptr;

    uint16_t m_w{}, m_h{};
    uint16_t m_default_element_w{}, m_default_element_h{};
    uint16_t texture_v_space{};

    bool m_is_loaded = false;
    std::vector<std::unique_ptr<element>> m_elements;

    uint16_t m_track_radius{};
    uint16_t m_max_mouse_movement{};
    float m_arrow_rot = 0.f;
};
