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
        if (settings->flags & (int) sources::history_flags::INCLUDE_PAD)
            std::lock_guard<std::mutex> lck3(gamepad::mutex);
        /* Check if the scroll wheel timed out if the data
         * is being fetched from the local data holder, since
         * scroll wheel doesn't have a released event */
        if (settings->data->is_local())
            hook::check_wheel();
        settings->data->populate_vector(m_inputs, settings);
    }
}

std::string input_entry::build_string(key_names* names, const bool use_fallback)
{
    static std::string plus = " + ";
    std::string result;
    const char* name = nullptr;

    for (const auto &key : m_inputs) {
        /* Either use custom names or builtin if option is enabled) */
        if (!names->empty() && (name = names->get_name(key))) {
            result.append(name).append(plus);
        } else if (use_fallback || names->empty()) {
            name = key_to_text(key);
#ifdef DEBUG
            if (!name) {
                /* If no name was found output the keycode */
                char buf[8];
                sprintf(buf, "0x%X", key);
                name = buf;
            }
#else
            if (name)
#endif
            result.append(name).append(plus);
        }
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
        if (settings->dir == history_direction::DOWN || settings->dir == history_direction::UP)
            temp.x = m_position.x + (i++) * (settings->h_space + icons->get_w());
        else
            temp.y = m_position.y + (i++) * (settings->v_space + icons->get_h());
        icons->draw(vc, &temp, this);
    }
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
    blog(LOG_DEBUG, "%s", buf.str().c_str());
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

void input_entry::render_effects()
{
    unsigned int rendered = 0;
    for (int priority = EFFECT_MAX_PRIORITY; priority >= 0; priority--) {
        for (auto &effect : m_effects) {
            if (effect->get_priority() >= priority) {
                effect->render();
                rendered++;
            }
        }
        if (rendered >= m_effects.size())
            break;
    }
}
