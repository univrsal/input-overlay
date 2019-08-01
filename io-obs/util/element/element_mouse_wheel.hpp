/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "element_texture.hpp"
#include "util/layout_constants.hpp"

#define WHEEL_MAP_MIDDLE  0
#define WHEEL_MAP_UP      1
#define WHEEL_MAP_DOWN    2

enum class wheel_data
{
    BOTH, BUTTON, WHEEL
};

class element_data_wheel : public element_data
{
public:
    element_data_wheel(wheel_direction dir, button_state state);

    explicit element_data_wheel(wheel_direction dir);

    explicit element_data_wheel(button_state state);

    wheel_direction get_dir() const;

    void set_dir(wheel_direction dir);

    button_state get_state() const;

    bool is_persistent() override;

    bool merge(element_data* other) override;

    wheel_data get_data_type() const;

private:
    wheel_data m_data_type;
    button_state m_middle_button;
    wheel_direction m_dir;
};

class element_wheel : public element_texture
{
public:
    element_wheel();

    void load(ccl_config* cfg, const std::string &id) override;

    void
    draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data, sources::overlay_settings* settings) override;

    data_source get_source() override;

private:
    /* Middle, Up, Down */
    gs_rect m_mappings[3];
};
