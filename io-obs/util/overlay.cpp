/**
 * This file is part of input-overlay
 * which is licensed under the MIT license.
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "../../ccl/ccl.hpp"
#include "overlay.hpp"
#include "layout_constants.hpp"
#include "element/element_button.hpp"
#include "element/element_data_holder.hpp"
#include "element/element.hpp"
#include "element/element_mouse_wheel.hpp"
#include "element/element_trigger.hpp"
#include "element/element_analog_stick.hpp"
#include "../sources/input_source.hpp"

//namespace Layout {

overlay::~overlay()
{
    unload();
}

overlay::overlay(sources::shared_settings* settings)
{
    m_settings = settings;
    m_is_loaded = load();
}

bool overlay::load()
{
    unload();
    m_is_loaded = load_texture() && load_cfg();
    return m_is_loaded;
}

void overlay::unload()
{
    unload_texture();
    unload_elements();
}

bool overlay::load_cfg()
{
    if (m_settings->layout_file.empty())
        return false;

    auto cfg = new ccl_config(m_settings->layout_file, "");
    auto flag = true;

    if (!cfg->has_fatal_errors())
    {
        m_default_size.x = static_cast<float>(cfg->get_int(CFG_DEFAULT_WIDTH)
            + CFG_OUTER_BORDER * 2);
        m_default_size.y = static_cast<float>(cfg->get_int(CFG_DEFAULT_HEIGHT)
            + CFG_OUTER_BORDER * 2);

        m_settings->cx = static_cast<uint32_t>(cfg->get_int(CFG_TOTAL_WIDTH));
        m_settings->cy = static_cast<uint32_t>(cfg->get_int(CFG_TOTAL_HEIGHT));

        auto element_id = cfg->get_string(CFG_FIRST_ID);

        while (!element_id.empty())
        {
            load_element(cfg, element_id);
            element_id = cfg->get_string(element_id + CFG_NEXT_ID, true);
        }
    }

    if (cfg->has_errors())
    {
        blog(LOG_WARNING, "[ccl] %s", cfg->get_error_message().c_str());
        if (cfg->has_fatal_errors())
        {
            flag = false;
        }
    }
    delete cfg;
    return flag;
}

bool overlay::load_texture()
{
    auto flag = true;
    if (!m_settings->image_file.empty())
    {
        if (m_image == nullptr)
        {
            m_image = new gs_image_file_t();
        }

        gs_image_file_init(m_image,  m_settings->image_file.c_str());

        obs_enter_graphics();
        gs_image_file_init_texture(m_image);
        obs_leave_graphics();

        if (!m_image->loaded)
        {
            blog(LOG_WARNING, "Error: failed to load texture %s",
                 m_settings->image_file.c_str());
            flag = false;
        }
        else
        {
            m_settings->cx = m_image->cx;
            m_settings->cy = m_image->cy;
        }
    }
    return flag;
}

void overlay::unload_texture() const
{
    obs_enter_graphics();
    gs_image_file_free(m_image);
    obs_leave_graphics();
}

void overlay::unload_elements()
{
    m_elements.clear();
}

void overlay::draw(gs_effect_t* effect)
{
    if (m_is_loaded)
    {
        for (auto const& element : m_elements)
        {
            element_data* data = nullptr;
            switch(element->get_source())
            {
            case GAMEPAD:
                data = hook::input_data->get_by_gamepad(m_settings->gamepad, element->get_keycode());
                break;
            case DEFAULT:
                data = hook::input_data->get_by_code(element->get_keycode());
                break;
            default: ;
            }
            element->draw(effect, m_image, data);
        }
    }
}

void overlay::load_element(ccl_config* cfg, const std::string& id)
{
    const auto type = cfg->get_int(id + CFG_TYPE);
    element* new_element = nullptr;
    switch (type)
    {
    case TEXTURE:
        new_element = new element_texture();
        break;
    case BUTTON:
        new_element = new element_button();
        break;
    case MOUSE_SCROLLWHEEL:
        new_element = new element_wheel();
        break;
    case TRIGGER:
        new_element = new element_trigger();
        break;
    case ANALOG_STICK:
        new_element = new element_analog_stick();
        break;
    default: ;
    }

    if (new_element)
    {
        new_element->load(cfg, id, &m_default_size);
        m_elements.emplace_back(new_element);
#ifdef _DEBUG
        blog(LOG_WARNING, "Added overlay element:\n Type: %i\n ID: %s\n ",
             new_element->get_type(), id.c_str());
#endif
    }
}

//};
