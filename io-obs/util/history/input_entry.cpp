/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/input-overlay
 */

#include "input_entry.hpp"
#include "../../sources/input_history.hpp"
#include "effect.hpp"
#include "../element/element_data_holder.hpp"
#include "key_names.hpp"
#include "history_icons.hpp"
#include "network/io_server.hpp"
#include <algorithm>
#include <sstream>

input_entry::input_entry(obs_source_t* source)
{
    m_text_source = source;
    m_position = {0.f, 0.f};

    m_width = obs_source_get_width(source);
    m_height = obs_source_get_height(source);
}

input_entry::input_entry()
{
    m_position = {0.f, 0.f};
}

input_entry::~input_entry()
{
    m_inputs.clear();
    m_effects.clear();
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
    m_position = {x, y};
}

void input_entry::set_text(const char* text, obs_data_t* settings)
{
    if (m_text_source) {
        obs_data_set_string(settings, "text", text);
        obs_source_update(m_text_source, settings);
    }
}

void input_entry::collect_inputs(sources::history_settings* settings)
{
    if (settings->data) {
        std::lock_guard<std::mutex> lck1(hook::mutex);
        std::lock_guard<std::mutex> lck2(network::mutex);
        if (settings->flags & FLAG_GAMEPAD)
            std::lock_guard<std::mutex> lck3(gamepad::mutex);

        settings->data->populate_vector(m_inputs, settings);
    }
}

std::string input_entry::build_string(key_names* names, const bool use_fallback)
{
    static std::string plus = " + ";
    std::string result;
    const char* name = nullptr;

    for (const auto &key : m_inputs) {
        if (!names->empty() && (name = names->get_name(key)))
            result += name + plus;
        else if (use_fallback || (names->empty() && (name = key_to_text(key))))
            result += name + plus;
    }

    /* Remove the last ' + '*/
    if (ends_with(result, plus))
        result.erase(result.length() - 3);

    return result;
}

void input_entry::tick(const float seconds)
{
    for (auto &effect : m_effects)
        effect->tick(seconds);

    /* Remove all finished effects safely */
    m_effects.erase(std::remove_if(m_effects.begin(), m_effects.end(), [](const std::unique_ptr<effect> &o)
    {
        return o->done();
    }), m_effects.end());
}

void input_entry::add_effect(effect* e)
{
    m_effects.emplace_back(e);
}

void input_entry::render_text()
{
    obs_source_video_render(m_text_source);
    m_width = obs_source_get_width(m_text_source);
    m_height = obs_source_get_height(m_text_source);
}

void input_entry::render_icons(sources::history_settings* settings, history_icons* icons)
{
    auto temp = m_position;
    auto i = 0;


    for (const auto& vc : m_inputs)
    {
        if (settings->dir == DIR_DOWN || settings->dir == DIR_UP)
            temp.x = m_position.x + (i++) * (settings->h_space + icons->get_w());
        else
            temp.y = m_position.y + (i++) * (settings->v_space + icons->get_h());
        icons->draw(vc, &temp);
    }

    gs_matrix_push();
    for (auto &effect : m_effects)
        effect->render();
    gs_matrix_pop();

}

void input_entry::clear()
{
    m_inputs.clear();
    m_effects.clear();
}

void input_entry::mark_for_removal()
{
    m_remove = true;
}

bool input_entry::finished() const
{
    return m_effects.empty() && m_remove;
}

bool input_entry::empty() const
{
    return m_inputs.empty();
}

void input_entry::test()
{
    std::stringstream buf;
    for (auto &input : m_inputs)
        buf << "0x" << std::hex << input << ", ";
    blog(LOG_INFO, "%s", buf.str().c_str());
}

input_entry::input_entry(input_entry &e)
{
    m_inputs = e.m_inputs;
}

void input_entry::clear_effects()
{
    m_effects.clear();
}

uint16_t input_entry::get_input_count() const
{
    return m_inputs.size();
}
