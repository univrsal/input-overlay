/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "../../sources/input_source.hpp"
#include "../../../ccl/ccl.hpp"
#include "element_dpad.hpp"
#include "../util.hpp"
#include "util/layout_constants.hpp"

element_dpad::element_dpad() : element_texture(DPAD_STICK)
{
}

void element_dpad::load(ccl_config* cfg, const std::string &id)
{
    element_texture::load(cfg, id);
    auto i = 1;
    for (auto &map : m_mappings) {
        map = m_mapping;
        map.x += i * (m_mapping.cx + CFG_INNER_BORDER);
        i++;
    }
    m_keycode = VC_DPAD_DATA;
}

void
element_dpad::draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data, sources::overlay_settings* settings)
{
    const auto d = dynamic_cast<element_data_dpad*>(data);

    if (d && d->get_direction() != DPAD_CENTER) {
        /* Enum starts at one (Center doesn't count)*/
        const auto map = &m_mappings[d->get_direction() - 1];
        element_texture::draw(effect, image, map);
    } else {
        element_texture::draw(effect, image, nullptr);
    }
    UNUSED_PARAMETER(settings);
}

data_source element_dpad::get_source()
{
    return GAMEPAD;
}

element_data_dpad::element_data_dpad(const dpad_direction a, const dpad_direction b) : element_data(DPAD_STICK)
{
    m_direction = a | b;
}

element_data_dpad::element_data_dpad(const dpad_direction d, const button_state state) : element_data(DPAD_STICK)
{
    m_direction = d;
    m_state = state;
}

bool element_data_dpad::is_persistent()
{
    return true;
}

void element_data_dpad::merge(element_data* other)
{
    const auto d = dynamic_cast<element_data_dpad*>(other);
#ifdef _WIN32
    if (d)
    {
        m_direction = d->m_direction;
    }
#else
    if (d) {
        if (d->get_state() == STATE_PRESSED) {
            m_direction |= d->m_direction;
        } else {
            m_direction &= ~d->m_direction;
        }
    }
#endif /* !WINDOWS*/
}

dpad_texture element_data_dpad::get_direction() const
{
    if (m_direction & DPAD_UP && m_direction & DPAD_LEFT)
        return DPAD_TEXTURE_TOP_LEFT;
    else if (m_direction & DPAD_UP && m_direction & DPAD_RIGHT)
        return DPAD_TEXTURE_TOP_RIGHT;
    else if (m_direction & DPAD_DOWN && m_direction & DPAD_LEFT)
        return DPAD_TEXTURE_BOTTOM_LEFT;
    else if (m_direction & DPAD_DOWN && m_direction & DPAD_RIGHT)
        return DPAD_TEXTURE_BOTTOM_RIGHT;
    else if (m_direction & DPAD_UP)
        return DPAD_TEXTURE_UP;
    else if (m_direction & DPAD_DOWN)
        return DPAD_TEXTURE_DOWN;
    else if (m_direction & DPAD_LEFT)
        return DPAD_TEXTURE_LEFT;
    else
        return DPAD_TEXTURE_RIGHT;
}

button_state element_data_dpad::get_state() const
{
    return m_state;
}
