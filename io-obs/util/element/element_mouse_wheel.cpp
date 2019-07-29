/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#include "../../sources/input_source.hpp"
#include "element_mouse_wheel.hpp"
#include "../util.hpp"
#include "../../hook/hook_helper.hpp"
#include "util/layout_constants.hpp"
#include <util/platform.h>

element_wheel::element_wheel() : element_texture(element_type::MOUSE_SCROLLWHEEL), m_mappings{}
{
    /* NO-OP */
}

void element_wheel::load(ccl_config* cfg, const std::string &id)
{
    element_texture::load(cfg, id);
    m_keycode = VC_MOUSE_WHEEL;
    auto i = 1;
    for (auto &map : m_mappings) {
        map = m_mapping;
        map.x += i * (m_mapping.cx + CFG_INNER_BORDER);
        i++;
    }
}

void element_wheel::draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data,
                         sources::overlay_settings* settings)
{
    if (data) {
        const auto wheel = dynamic_cast<element_data_wheel*>(data);

        if (wheel) {
            if (wheel->get_state() == button_state::PRESSED)
                element_texture::draw(effect, image, &m_mappings[WHEEL_MAP_MIDDLE]);

            switch (wheel->get_dir()) {
                case wheel_direction::UP:
                    element_texture::draw(effect, image, &m_mappings[WHEEL_MAP_UP]);
                    break;
                case wheel_direction::DOWN:
                    element_texture::draw(effect, image, &m_mappings[WHEEL_MAP_DOWN]);
                    break;
                case wheel_direction::NONE:;
            }
        }
    }

    element_texture::draw(effect, image, data, settings);
}

data_source element_wheel::get_source()
{
    return data_source::DEFAULT;
}

element_data_wheel::element_data_wheel(const wheel_direction dir, const button_state state) : element_data_wheel(dir)
{
    m_data_type = wheel_data::BOTH;
    m_middle_button = state;
}

element_data_wheel::element_data_wheel(const wheel_direction dir) : element_data(element_type::MOUSE_SCROLLWHEEL),
                                                                    m_middle_button(button_state::RELEASED)
{
    m_data_type = wheel_data::WHEEL;
    m_dir = dir;
}

element_data_wheel::element_data_wheel(const button_state state) : element_data(element_type::MOUSE_SCROLLWHEEL), m_dir()
{
    m_data_type = wheel_data::BUTTON;
    m_middle_button = state;
}


wheel_direction element_data_wheel::get_dir() const
{
    return m_dir;
}

void element_data_wheel::set_dir(const wheel_direction dir)
{
    m_dir = dir;
}

button_state element_data_wheel::get_state() const
{
    return m_middle_button;
}

bool element_data_wheel::is_persistent()
{
    return true;
}

void element_data_wheel::merge(element_data* other)
{
    if (other && other->get_type() == m_type) {
        const auto other_wheel = dynamic_cast<element_data_wheel*>(other);

        if (other_wheel) {
            /* After the merge this data contains both the scroll wheel state and the button state */
            if (m_data_type != other_wheel->m_data_type)
                m_data_type = wheel_data::BOTH;

            switch (other_wheel->m_data_type) {
                case wheel_data::BUTTON:
                    m_middle_button = other_wheel->get_state();
                    break;
                case wheel_data::WHEEL:
                    m_dir = other_wheel->get_dir();
                    break;
                case wheel_data::BOTH:
                    m_dir = other_wheel->get_dir();
                    m_middle_button = other_wheel->get_state();
            }
        }
    }
}

wheel_data element_data_wheel::get_data_type() const
{
    return m_data_type;
}
