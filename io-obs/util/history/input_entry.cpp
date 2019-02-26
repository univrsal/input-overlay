/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "input_entry.hpp"
#include "../../sources/input_history.hpp"
#include "effect.hpp"
#include "../element/element_data_holder.hpp"
#include "key_names.hpp"
#include "history_icons.hpp"
#include <algorithm>

input_entry::input_entry(obs_source_t* source)
{
    m_text_source = source;
    m_position = { 0.f, 0.f };

    m_width = obs_source_get_width(source);
    m_height = obs_source_get_height(source);
}

input_entry::input_entry()
{
    m_position = { 0.f, 0.f };
}

input_entry::~input_entry()
{
    m_inputs.clear();
}

uint16_t input_entry::get_width() const
{
    return m_width;
}

uint16_t input_entry::get_height() const
{
    return m_height;
}

vec2* input_entry::get_pos()
{
    return &m_position;
}

void input_entry::set_pos(float x, float y)
{
    m_position = { x, y };
}

void input_entry::set_text(const char* text, obs_data_t* settings)
{
    if (m_text_source)
    {
        obs_data_set_string(settings, "text", text);
        obs_source_update(m_text_source, settings);
        //obs_source_update(settings->source, settings);

        m_width = obs_source_get_width(m_text_source);
        m_height = obs_source_get_height(m_text_source);
    }
}

void input_entry::collect_inputs(sources::history_settings* settings)
{
    if (settings->data)
        settings->data->populate_vector(m_inputs, settings);
}

std::string input_entry::build_string(key_names* names, const bool use_fallback)
{
    static std::string plus = " + ";
    std::string result;
    const char* name = nullptr;

    for (const auto& key : m_inputs)
    {
        if (names && (name = names->get_name(key)))
            result += name + plus;
        else if (use_fallback || !names)
            result += key_to_text(key) + plus;
    }

    /* Remove the last ' + '*/
    result.erase(result.length() - 3);

    return result;
}

void input_entry::tick(const float seconds)
{
    for (auto& effect : m_effects)
        effect->tick(seconds);

    /* Remove all finished effects safely */
    m_effects.erase(std::remove_if(
        m_effects.begin(), m_effects.end(),
        [](const std::unique_ptr<effect>& o)
    {
#ifdef _DEBUG
        if (o->done())
        {
            blog(LOG_DEBUG, "Effect finished.");
            return true;
        }
        return false;
#else
        return o->done();
#endif
    }
    ), m_effects.end());
}

void input_entry::add_effect(effect* e)
{
    m_effects.emplace_back(e);
}

void input_entry::render_text()
{
    gs_matrix_push();

    obs_source_video_render(m_text_source);

    for (auto& effect : m_effects)
        effect->render();
    gs_matrix_pop();
}

void input_entry::render_icons(sources::history_settings* settings)
{
    gs_matrix_push();
    auto temp = m_position;
    auto i = 0;

    for (const auto& vc : m_inputs)
    {
        settings->icons->draw(vc, &temp);

        if (settings->dir == DIR_DOWN || settings->dir == DIR_UP)
            temp.x += i++ * (settings->h_space + settings->icons->get_w());
        else        
            temp.y += i++ * (settings->v_space + settings->icons->get_h());
    }
    
    for (auto& effect : m_effects)
        effect->render();
    gs_matrix_pop();
}

void input_entry::mark_for_removal()
{
    m_remove = true;
}

bool input_entry::finished() const
{
    return m_effects.empty() && m_remove;
}

bool input_entry::effects_finished() const
{
    return m_effects.empty();
}

bool input_entry::empty() const
{
    return m_inputs.empty();
}

void input_entry::test()
{
    for (auto& input : m_inputs)
        blog(LOG_INFO, "0x%X ", input);
}
