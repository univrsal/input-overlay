/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "element_texture.hpp"
#include <netlib.h>

enum class trigger_data
{
    NONE = -1,
    BOTH,
    LEFT,
    RIGHT
};

enum class trigger_direction;

enum class element_side;

/* Contains data for both trigger buttons
 */
class element_data_trigger : public element_data
{
public:
    /*
        Separate constructors are used on linux
        because the values can't be queried together
    */
    element_data_trigger(trigger_data side, float val);

    element_data_trigger(float left, float right);

    float get_left() const;

    float get_right() const;

    bool is_persistent() override;

    bool merge(element_data* other) override;

    static element_data_trigger* from_buffer(netlib_byte_buf* buffer);

private:
    trigger_data m_data_type = trigger_data::BOTH;
    float m_left_trigger = 0.f, m_right_trigger = 0.f;
};

class element_trigger : public element_texture
{
public:
    element_trigger();

    void load(ccl_config* cfg, const std::string& id) override;

    void draw(gs_effect_t* effect, gs_image_file_t* image,
        element_data* data, sources::overlay_settings* settings) override;

    data_source get_source() override;

private:
    void calculate_mapping(gs_rect* pressed, vec2* pos, float progress) const;
    gs_rect m_pressed;
    element_side m_side;
    trigger_direction m_direction;
    bool m_button_mode = false;
};
