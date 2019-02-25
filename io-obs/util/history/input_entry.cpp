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

/* Maps a specific mask value to a keycode
 * libuiohook repports events with masks
 * which can be used to detect key combinations
 * e.g. when the users presses Ctr+C, the event
 * will have the keycode C with the mask MASK_CTRL_L
 * (if the left control key was used)
 * Since input-history translates keycodes into text
 * this maps each mask to the respective keycode
 * for easier translation
 */
struct mask_vc
{
    uint16_t mask, vc;
};

const std::string plus = " + ";
mask_vc mask_vc_map[] = { {MASK_ALT_L, VC_ALT_L}, {MASK_ALT_R, VC_ALT_R},
    {MASK_CTRL_L, VC_CONTROL_L}, {MASK_CTRL_R, VC_CONTROL_R},
    {MASK_SHIFT_L, VC_SHIFT_L}, {MASK_SHIFT_R, VC_SHIFT_R},
    {MASK_META_L, VC_META_L}, {MASK_META_R, VC_META_R},
    {MASK_CAPS_LOCK, VC_CAPS_LOCK}, {MASK_NUM_LOCK, VC_NUM_LOCK},
    {MASK_SCROLL_LOCK, VC_SCROLL_LOCK}, {MASK_BUTTON1, VC_MOUSE_BUTTON1},
    {MASK_BUTTON2, VC_MOUSE_BUTTON2}, {MASK_BUTTON3, VC_MOUSE_BUTTON3},
    {MASK_BUTTON4, VC_MOUSE_BUTTON4}, {MASK_BUTTON5, VC_MOUSE_BUTTON5}};

void input_entry::mask_to_string(std::string& str, uint16_t mask, bool use_fallback, key_names* names)
{
    const char* name = nullptr;
    
    for (const auto& m : mask_vc_map)
    {
        if (mask & m.mask)
        {
            if ((name = names->get_name(m.vc)))
                str += name + plus;
            else if (use_fallback)
                str += key_to_text(m.vc) + plus;
        }
    }
}

input_entry::input_entry() : m_mask(0)
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

void input_entry::collect_inputs(sources::history_settings* settings)
{
    if (settings->data)
        settings->data->populate_vector(m_inputs, settings);
}

void input_entry::build_string(sources::history_settings* settings)
{
    const bool use_fallback = settings->flags & sources::FLAG_USE_FALLBACK;
    const char* name = nullptr;

    if (settings->flags & sources::FLAG_USE_MASKS)
        mask_to_string(m_text, m_mask, use_fallback, settings->names);

    for (const auto& key : m_inputs)
    {
        if (settings->names && (name = settings->names->get_name(key)))
            m_text += name + plus;
        else if (use_fallback || !settings->names)
            m_text += key_to_text(key) + plus;
    }

    /* Remove the last ' + '*/
    m_text.erase(m_text.length() - 3);
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

void input_entry::render_text(sources::history_settings* settings)
{
    //gs_matrix_push();
    blog(LOG_INFO, "drawing: %s", m_text.c_str());

    obs_data_set_string(settings->settings, "text", m_text.c_str());
    obs_source_update(settings->text_source, settings->settings);
    obs_source_update(settings->source, settings->settings);

    if (m_width == 0 || m_height == 00)
    {
        m_width = obs_source_get_width(settings->text_source);
        m_height = obs_source_get_height(settings->text_source);
    }

    obs_source_video_render(settings->text_source);

  /*  for (auto& effect : m_effects)
        effect->render();*/
    //gs_matrix_pop();
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

bool input_entry::empty() const
{
    return m_inputs.empty();
}

void input_entry::test()
{
    for (auto& input : m_inputs)
        blog(LOG_INFO, "0x%X ", input);
}
