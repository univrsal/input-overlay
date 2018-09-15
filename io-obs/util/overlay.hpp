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

extern "C" {
#include <graphics/image-file.h>
}

#include "../hook/hook_helper.hpp"

//namespace Data {

class overlay
{
public:
    overlay() = default;

    ~overlay();

    overlay(std::string* texture, std::string* config, uint32_t* cx,
            uint32_t* cy);

    bool load();

    void unload();

    void draw(gs_effect_t* effect);

    void update_settings(obs_data_t* settings);

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
    void load_element(ccl_config* cfg, const std::string& id);

    gs_image_file_t* m_image = nullptr;

    uint32_t *m_w{}, *m_h{};
    vec2 m_default_size{};

    std::string* m_image_file;
    std::string* m_layout_file;

    bool m_is_loaded = false;
    std::vector<std::unique_ptr<element>> m_elements;

    uint16_t m_track_radius{};
    uint16_t m_max_mouse_movement{};
    float m_arrow_rot = 0.f;
};
