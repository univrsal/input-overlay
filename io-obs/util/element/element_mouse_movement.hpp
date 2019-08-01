/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "element_texture.hpp"
#include "util/layout_constants.hpp"

/* Contains all information about mouse movement,
 * button clicks and scroll motion
 */
class element_data_mouse_pos : public element_data
{
public:
    element_data_mouse_pos(int16_t x, int16_t y);

    bool is_persistent() override;
    bool merge(element_data* other) override;
    float get_mouse_angle(sources::overlay_settings* settings);
    void get_mouse_offset(sources::overlay_settings* settings, const vec2 &center, vec2 &out, uint8_t radius) const;
    int16_t get_mouse_x() const;
    int16_t get_mouse_y() const;

private:

    int16_t m_x{}, m_y{};
    int16_t m_last_x{}, m_last_y{};
    float m_old_angle{};
};

class element_mouse_movement : public element_texture
{
public:
    element_mouse_movement();

    void load(ccl_config* cfg, const std::string &id) override;

    void
    draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data, sources::overlay_settings* settings) override;

    data_source get_source() override
    { return data_source::MOUSE_POS; }

private:
    mouse_movement m_movement_type;
    vec2 m_offset_pos = {};
    uint8_t m_radius = 0;
};
