/**
 * This file is part of input-overlay
 * which is licensed under the MIT license.
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "overlay.hpp"
#include "layout_constants.hpp"
#include "element/element_button.hpp"
#include "element/element_data_holder.hpp"
#include "element/element.hpp"
#include "element/element_mouse_wheel.hpp"

//namespace Layout {

overlay::~overlay()
{
    unload();
}

overlay::overlay(const std::string& ini, const std::string& texture)
{
    load_texture(texture);
    load_cfg(ini);
}

bool overlay::load(const std::string& cfg_path, const std::string& texture_path)
{
    unload();
    m_is_loaded = load_texture(texture_path) && load_cfg(cfg_path);
    return m_is_loaded;
}

void overlay::unload()
{
    unload_texture();
    unload_elements();
}

bool overlay::load_cfg(const std::string& path)
{
    if (path.empty())
        return false;
    auto cfg = new ccl_config(path, "");
    auto flag = true;

    if (!cfg->has_fatal_errors())
    {
        m_default_element_h = cfg->get_int(CFG_DEFAULT_HEIGHT);
        m_default_element_w = cfg->get_int(CFG_DEFAULT_WIDTH);

        m_w = cfg->get_int(CFG_TOTAL_WIDTH);
        m_h = cfg->get_int(CFG_TOTAL_HEIGHT);

        auto element_id = cfg->get_string(CFG_FIRST_ID);

        while (!element_id.empty())
        {
            load_element(cfg, element_id);
            element_id = cfg->get_string(element_id + CFG_NEXT_ID);
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
    cfg = nullptr;
    return flag;
}

bool overlay::load_texture(const std::string& path)
{
    auto flag = true;
    if (!path.empty())
    {
        if (m_image == nullptr)
        {
            m_image = new gs_image_file_t();
        }

        gs_image_file_init(m_image, path.c_str());

        obs_enter_graphics();
        gs_image_file_init_texture(m_image);
        obs_leave_graphics();

        if (!m_image->loaded)
        {
            blog(LOG_WARNING, "Error: failed to load texture %s", path.c_str());
            flag = false;
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
            if (element->get_type() != TEXTURE)
            {
                data = hook::input_data->get_by_code(element->get_keycode());
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
    default: ;
    }

    if (new_element)
    {
        new_element->load(cfg, id);
        m_elements.emplace_back(new_element);
#ifdef _DEBUG
        blog(LOG_WARNING, "Added overlay element:\n Type: %i\n ID: %s\n ", new_element->get_type(), id.c_str());
#endif
    }
}

//};
